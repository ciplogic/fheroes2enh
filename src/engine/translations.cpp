/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <map>

#include "engine.h"
#include "serialize.h"
#include "system.h"
#include "tools.h"
#include <fstream>


namespace ModernTranslation
{
    using StringVector = std::vector <string>;
    using StringTable = std::map<std::string, std::string>;
    struct TranslationTable
    {
        StringTable directTranslations;
        StringTable singularTranslation;
        StringTable pluralTranslation;

    public:
        void clear();
        bool readFile(const std::string& fileName) const;
        void addDirectTranslation(StringVector& id, StringVector& trans);
        void addMultiTranslation(StringVector& messageIds, StringVector& messagePlurals, StringVector& messageStr1, StringVector& messageStr2);
        
        string getTranslation(const string& str) const;
        string getTranslationPlural(const string& singular, const string& plural, size_t count) const;

    private:
        static std::string joinAsRows(StringVector& id);
    };
    namespace
    {
        std::string getDictText(const StringTable& stringTable, const std::string& item)
        {
            const auto findIt = stringTable.find(item);
            std::string result;
            if (findIt == stringTable.end())
                result = item;
            else result = findIt->second;
            const auto posFind = result.find('|');
            if (posFind == std::string::npos)
                return result;
            result = result.substr(posFind+1);
            return result;
        }
    }
    void TranslationTable::clear()
    {
        directTranslations.clear();
        singularTranslation.clear();
        pluralTranslation.clear();
    }

    enum class TranslationLineType
    {
        Empty,
        Msgid,
        Msgid_plural,
        Msgstr,
        QuoteLine,
        Msgstr1,
        Msgstr2
    };

    struct ClassifiedLine
    {
        TranslationLineType translationLineType{};
        std::string line;
    };

    bool startsWith(const string& text, const string& subStr)
    {
        if(text.size()< subStr.size())
            return false;
        const auto subText = text.substr(0, subStr.size());
        return subText == subStr;
    }

    bool cutTextIfMatchStart(const string& text, const string& subStr, string& outStr)
    {
        outStr = "";
        if(!startsWith(text, subStr))
            return false;
        outStr = text.substr(subStr.size()+1);
        outStr.pop_back();
        return true;
    }

    ClassifiedLine classifyLine(string text)
    {
        ClassifiedLine result;
        if(text.empty())
        {
            result.translationLineType = TranslationLineType::Empty;
            return result;
        }
        if(text[0] == '#')
        {
            result.translationLineType = TranslationLineType::Empty;
            result.line = text.substr(1);
            return result;
        }
        string outRow;
        if (text[0] == '"')
        {
            cutTextIfMatchStart(text, "", outRow);
            result.translationLineType = TranslationLineType::QuoteLine;
            result.line = outRow;
            return result;
        }
        if(cutTextIfMatchStart(text, "msgid ", outRow))
        {
            result.translationLineType = TranslationLineType::Msgid;
            result.line = outRow;
            return result;
        }
        if (cutTextIfMatchStart(text, "msgstr ", outRow))
        {
            result.translationLineType = TranslationLineType::Msgstr;
            result.line = outRow;
            return result;
        }

        if (cutTextIfMatchStart(text, "msgid_plural ", outRow))
        {
            result.translationLineType = TranslationLineType::Msgid_plural;
            result.line = outRow;
            return result;
        }

        if (cutTextIfMatchStart(text, "msgstr[0] ", outRow))
        {
            result.translationLineType = TranslationLineType::Msgstr1;
            result.line = outRow;
            return result;
        }

        if (cutTextIfMatchStart(text, "msgstr[1] ", outRow))
        {
            result.translationLineType = TranslationLineType::Msgstr2;
            result.line = outRow;
            return result;
        }

        return result;
    }

    TranslationTable mainTable;

    struct BuildVector
    {
        StringVector messageIds, messagePlurals, messageStr, messageStr1, messageStr2;

        void clear()
        {
            messageIds.clear();
            messageStr.clear();
            messageStr1.clear();  
            messageStr2.clear();
        }
        void addString(TranslationLineType lineType, const std::string& line)
        {
            switch (lineType)
            {
            case TranslationLineType::Msgid:
                messageIds.push_back(line);
                break;
            case TranslationLineType::Msgid_plural:
                messagePlurals.push_back(line);
                break;
            case TranslationLineType::Msgstr:
                messageStr.push_back(line);
                break;
            case TranslationLineType::Msgstr1:
                messageStr1.push_back(line);
                break;
            case TranslationLineType::Msgstr2:
                messageStr2.push_back(line);
                break;
            }
        }
    };

    bool TranslationTable::readFile(const std::string& fileName) const
    {
        std::ifstream infile(fileName);
        if(!infile.is_open())
            return false;
        std::string line;
        TranslationLineType current = TranslationLineType::Empty;
        BuildVector buildVector;
        while (std::getline(infile, line))
        {
            const ClassifiedLine classLine = classifyLine(line);
            if (classLine.translationLineType == TranslationLineType::QuoteLine) {
                buildVector.addString(current, classLine.line);
                continue;
            }
            if (classLine.translationLineType == TranslationLineType::Empty)
            {
                if (current == TranslationLineType::Msgstr)
                {
                    mainTable.addDirectTranslation(buildVector.messageIds, buildVector.messageStr);
                    buildVector.clear();
                    continue;
                }
                if (current == TranslationLineType::Msgstr2)
                {
                    mainTable.addMultiTranslation(buildVector.messageIds, buildVector.messagePlurals, 
                        buildVector.messageStr1, buildVector.messageStr2);
                    buildVector.clear();
                    continue;
                }
                continue;
            }
            buildVector.addString(classLine.translationLineType, classLine.line);
            current = classLine.translationLineType;
        
        }

        return true;
    }

    void TranslationTable::addDirectTranslation(StringVector& id, StringVector& trans)
    {
        string key = joinAsRows(id);
        string value = joinAsRows(trans);
        if(key.empty() || value.empty())
            return;
        directTranslations[key] = value;
    }

    void TranslationTable::addMultiTranslation(StringVector& messageIds, StringVector& messagePlurals,
        StringVector& messageStr1, StringVector& messageStr2)
    {
        string key = joinAsRows(messageIds);
        string value = joinAsRows(messageStr1);
        if (key.empty() || value.empty())
            return;
        singularTranslation[key] = value;
        string keyPl = joinAsRows(messagePlurals);
        string valuePl = joinAsRows(messageStr2);
        if (keyPl.empty() || valuePl.empty())
            return;
        pluralTranslation[keyPl] = valuePl;
    }

    string TranslationTable::getTranslation(const string& str) const
    {
        auto result = getDictText(directTranslations, str);
    

        return result;
    }
    string TranslationTable::getTranslationPlural(const string& singular, const string& plural, size_t count) const
    {
        if (count == 1) {
            return getDictText(singularTranslation, singular);
        }
        return getDictText(pluralTranslation, plural);
    }

    std::string TranslationTable::joinAsRows(StringVector& id)
    {
        string result;

        if (id.empty())
            return result;
        if(id[0].empty())
        {
            id.erase(id.begin());
        }
        
        bool first = true;
        for (const auto& it : id)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                result += '\n';
            }
            result += it;
        }
        return result;
    }

}

namespace Translation
{
    bool bindDomain(const char *file)
    {
        return  ModernTranslation::mainTable.readFile(file);
    }
    const string gettext(const string &str)
    {
        return ModernTranslation::mainTable.getTranslation(str);
    }

    const string ngettext(const char *str, const char *plural, size_t n)
    {
        return ModernTranslation::mainTable.getTranslationPlural(str, plural, n);
    }

    const string dngettext(const char *domain, const char *str, const char *plural, size_t num)
    {
        return ngettext(str, plural, num);
    }
}
