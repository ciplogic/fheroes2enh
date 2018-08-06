/***************************************************************************
 *   Copyright (C) 2012 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#pragma once

#include <utility>
#include <algorithm>
#include "gamedefs.h"
#include "display.h"
#include "tools.h"

namespace Interface
{
    template<class Item>
    class ItemsBar
    {
    protected:
        typedef vector<Item *> Items;
        typedef typename vector<Item *>::iterator ItemsIterator;
        typedef pair<ItemsIterator, Rect> ItemIterPos;

        Items items;
        Rect barsz;
        Size itemsz;
        Size colrows;
        int hspace;
        int vspace;

    public:
        ItemsBar() : colrows(0, 0), hspace(0), vspace(0)
        {}

        virtual ~ItemsBar() = default;

        /*
	void		SetColRows(uint32_t, uint32_t);
	void        	SetPos(s32, s32);
	void        	SetItemSize(uint32_t, uint32_t);
	void		SetHSpace(int);
	void		SetVSpace(int);
	void		SetContent(const std::list<Item> &);
	void		SetContent(const std::vector<Item> &);
	
	const Point &	GetPos() const;
	const Rect &	GetArea() const;
	const Size &	GetColRows() const;
	Item*		GetItem(const Point &) const;

	void		Redraw(Surface & dstsf = Display::Get());
	bool		QueueEventProcessing();
*/

        virtual void RedrawBackground(const Rect &, Surface &)
        {}

        virtual void RedrawItem(Item &, const Rect &, Surface &)
        {}

        virtual bool ActionBarSingleClick(const Point &, Item &item, const Rect &)
        { return false; }

        virtual bool ActionBarPressRight(const Point &, Item &item, const Rect &)
        { return false; }

        virtual bool ActionBarCursor(const Point &, Item &, const Rect &)
        { return false; }

        //body
        void SetColRows(uint32_t col, uint32_t row)
        {
            colrows.w = col;
            colrows.h = row;
            RescanSize();
        }

        void SetContent(list<Item> &content)
        {
            items.clear();
            for (typename list<Item>::iterator
                         it = content.begin(); it != content.end(); ++it)
                items.push_back(&(*it));
            SetContentItems();
        }

        void SetContent(vector<Item> &content)
        {
            items.clear();
            for (typename vector<Item>::iterator
                         it = content.begin(); it != content.end(); ++it)
                items.push_back(&(*it));
            SetContentItems();
        }

        void SetPos(s32 px, s32 py)
        {
            barsz.x = px;
            barsz.y = py;
        }

        void SetItemSize(uint32_t pw, uint32_t ph)
        {
            itemsz.w = pw;
            itemsz.h = ph;
            RescanSize();
        }

        void SetHSpace(int val)
        {
            hspace = val;
            RescanSize();
        }

        void SetVSpace(int val)
        {
            vspace = val;
            RescanSize();
        }

        Item *GetItem(const Point &pt)
        {
            ItemsIterator posItem = GetItemIter(pt);
            return posItem != items.end() ? *posItem : nullptr;
        }

        Rect *GetItemPos(const Point &pt)
        {
            ItemIterPos posItem = GetItemIterPos(pt);
            return posItem.first != items.end() ? &posItem.second : nullptr;
        }

        s32 GetIndex(const Point &pt)
        {
            ItemsIterator posItem = GetItemIter(pt);
            return posItem != items.end() ? std::distance(items.end(), posItem) : -1;
        }

        const Point &GetPos() const
        {
            return barsz;
        }

        const Rect &GetArea() const
        {
            return barsz;
        }

        const Size &GetColRows() const
        {
            return colrows;
        }

        virtual void Redraw(Surface &dstsf = Display::Get())
        {
            Point dstpt(barsz);

            for (uint32_t yy = 0; yy < colrows.h; ++yy)
            {
                for (uint32_t xx = 0; xx < colrows.w; ++xx)
                {
                    RedrawBackground(Rect(dstpt, itemsz.w, itemsz.h), dstsf);

                    dstpt.x += hspace + itemsz.w;
                }

                dstpt.x = barsz.x;
                dstpt.y += vspace + itemsz.h;
            }

            dstpt = barsz;
            auto posItem = GetTopItemIter();

            for (uint32_t yy = 0; yy < colrows.h; ++yy)
            {
                for (uint32_t xx = 0; xx < colrows.w; ++xx)
                {
                    if (posItem != items.end())
                    {
                        RedrawItemIter(posItem, Rect(dstpt, itemsz.w, itemsz.h), dstsf);

                        ++posItem;
                    }

                    dstpt.x += hspace + itemsz.w;
                }

                dstpt.x = barsz.x;
                dstpt.y += vspace + itemsz.h;
            }
        }

        bool QueueEventProcessing()
        {
            const Point &cursor = LocalEvent::Get().GetMouseCursor();

            return !isItemsEmpty() && ActionCursorItemIter(cursor, GetItemIterPos(cursor));
        }

    protected:
        virtual void SetContentItems()
        {
        }

        ItemsIterator GetBeginItemIter()
        {
            return items.begin();
        }

        ItemsIterator GetEndItemIter()
        {
            return items.end();
        }

        virtual ItemsIterator GetTopItemIter()
        {
            return items.begin();
        }

        virtual ItemsIterator GetCurItemIter()
        {
            return items.end();
        }

        virtual void RedrawItemIter(ItemsIterator it, const Rect &pos, Surface &dstsf)
        {
            RedrawItem(**it, pos, dstsf);
        }

        virtual bool ActionCursorItemIter(const Point &cursor, ItemIterPos iterPos)
        {
            if (iterPos.first != GetEndItemIter())
            {
                LocalEvent &le = LocalEvent::Get();

                if (ActionBarCursor(cursor, **iterPos.first, iterPos.second))
                    return true;
                if (le.MouseClickLeft(iterPos.second))
                    return ActionBarSingleClick(cursor, **iterPos.first, iterPos.second);
                if (le.MousePressRight(iterPos.second))
                    return ActionBarPressRight(cursor, **iterPos.first, iterPos.second);
            }

            return false;
        }

        bool isItemsEmpty()
        {
            return items.empty();
        }

        ItemsIterator GetItemIter(const Point &pt)
        {
            return GetItemIterPos(pt).first;
        }

        ItemIterPos GetItemIterPos(const Point &pt)
        {
            Rect dstrt(barsz, itemsz.w, itemsz.h);
            auto posItem = GetTopItemIter();

            for (uint32_t yy = 0; yy < colrows.h; ++yy)
            {
                for (uint32_t xx = 0; xx < colrows.w; ++xx)
                {
                    if (posItem != items.end())
                    {
                        if (dstrt & pt)
                            return ItemIterPos(posItem, dstrt);
                        ++posItem;
                    }

                    dstrt.x += hspace + itemsz.w;
                }

                dstrt.x = barsz.x;
                dstrt.y += vspace + itemsz.h;
            }

            return std::pair<ItemsIterator, Rect>(items.end(), Rect());
        }

    private:
        void RescanSize()
        {
            barsz.w = colrows.w ? colrows.w * itemsz.w + (colrows.w - 1) * hspace : 0;
            barsz.h = colrows.h ? colrows.h * itemsz.h + (colrows.h - 1) * vspace : 0;
        }
    };

    template<class Item>
    class ItemsActionBar : public ItemsBar<Item>
    {
    protected:
        typedef typename ItemsBar<Item>::ItemsIterator ItemsIterator;
        typedef typename ItemsBar<Item>::ItemIterPos ItemIterPos;

        ItemsIterator topItem;
        ItemIterPos curItemPos;

    public:
        ItemsActionBar()
        {
            ResetSelected();
        }

        virtual ~ItemsActionBar()
        = default;

        /*
	Item*		GetSelectedItem();
	Rect*		GetSelectedPos();
	s32		GetSelectedIndex();

	bool		isSelected();
	void		ResetSelected();

        bool QueueEventProcessing();
	bool QueueEventProcessing(ItemsActionBar<Item> &);
*/

        virtual void RedrawItem(Item &, const Rect &, Surface &)
        {}

        virtual void RedrawItem(Item &, const Rect &, bool, Surface &)
        {}

        virtual bool ActionBarSingleClick(const Point &, Item &, const Rect &, Item &, const Rect &)
        { return false; }

        virtual bool ActionBarPressRight(const Point &, Item &, const Rect &, Item &, const Rect &)
        { return false; }

        virtual bool ActionBarSingleClick(const Point &, Item &item, const Rect &)
        { return false; }

        virtual bool ActionBarDoubleClick(const Point &cursor, Item &item, const Rect &pos)
        { return ActionBarSingleClick(cursor, item, pos); }

        virtual bool ActionBarPressRight(const Point &, Item &item, const Rect &)
        { return false; }

        virtual bool ActionBarCursor(const Point &, Item &, const Rect &)
        { return false; }

        virtual bool ActionBarCursor(const Point &, Item &, const Rect &, Item &, const Rect &)
        { return false; }

        // body
        Item *GetSelectedItem()
        {
            return *GetCurItemIter();
        }

        Rect *GetSelectedPos()
        {
            return &curItemPos.second;
        }

        s32 GetSelectedIndex()
        {
            return std::distance(ItemsBar<Item>::GetBeginItemIter(), GetCurItemIter());
        }

        bool isSelected()
        {
            return GetCurItemIter() != ItemsBar<Item>::GetEndItemIter();
        }

        virtual void ResetSelected()
        {
            topItem = ItemsBar<Item>::GetBeginItemIter();
            curItemPos = ItemIterPos(ItemsBar<Item>::items.end(), Rect());
        }

        bool QueueEventProcessing()
        {
            return ItemsBar<Item>::QueueEventProcessing();
        }

        bool QueueEventProcessing(ItemsActionBar<Item> &other)
        {
            const Point &cursor = LocalEvent::Get().GetMouseCursor();

            if (ItemsBar<Item>::isItemsEmpty() && other.isItemsEmpty())
                return false;

            return other.isSelected() ?
                   ActionCursorItemIter(cursor, other) : ActionCursorItemIter(cursor,
                                                                              ItemsBar<Item>::GetItemIterPos(cursor));
        }

    protected:
        ItemsIterator GetTopItemIter()
        {
            return topItem;
        }

        ItemsIterator GetCurItemIter()
        {
            return curItemPos.first;
        }

        void SetContentItems()
        {
            ResetSelected();
        }

        void RedrawItemIter(ItemsIterator it, const Rect &pos, Surface &dstsf)
        {
            RedrawItem(**it, pos, GetCurItemIter() == it, dstsf);
        }

        bool ActionCursorItemIter(const Point &cursor, ItemIterPos iterPos)
        {
            if (iterPos.first != ItemsBar<Item>::GetEndItemIter())
            {
                LocalEvent &le = LocalEvent::Get();

                if (ActionBarCursor(cursor, **iterPos.first, iterPos.second))
                    return true;
                else if (le.MouseClickLeft(iterPos.second))
                {
                    if (iterPos.first == GetCurItemIter())
                    {
                        return ActionBarDoubleClick(cursor, **iterPos.first, iterPos.second);
                    } else
                    {
                        if (ActionBarSingleClick(cursor, **iterPos.first, iterPos.second))
                            curItemPos = iterPos;
                        else
                            ResetSelected();

                        return true;
                    }
                } else if (le.MousePressRight(iterPos.second))
                    return ActionBarPressRight(cursor, **iterPos.first, iterPos.second);
            }

            return false;
        }

        bool ActionCursorItemIter(const Point &cursor, ItemsActionBar<Item> &other)
        {
            ItemIterPos iterPos1 = ItemsBar<Item>::GetItemIterPos(cursor);
            ItemIterPos iterPos2 = other.curItemPos;

            if (iterPos1.first != ItemsBar<Item>::GetEndItemIter())
            {
                LocalEvent &le = LocalEvent::Get();

                if (ActionBarCursor(cursor, **iterPos1.first, iterPos1.second, **iterPos2.first, iterPos2.second))
                    return true;
                if (le.MouseClickLeft(iterPos1.second))
                {
                    if (ActionBarSingleClick(cursor, **iterPos1.first, iterPos1.second, **iterPos2.first,
                                             iterPos2.second))
                        curItemPos = iterPos1;
                    else
                        ResetSelected();

                    other.ResetSelected();
                    return true;
                }
                if (le.MousePressRight(iterPos1.second))
                {
                    other.ResetSelected();
                    return ActionBarPressRight(cursor, **iterPos1.first, iterPos1.second, **iterPos2.first,
                                               iterPos2.second);
                }
            }

            return false;
        }
    };
}

#include "button.h"
#include "splitter.h"

namespace Interface
{
    template<class Item>
    class ItemsScroll : public ItemsActionBar<Item>
    {
    protected:
        Button buttonPgUp;
        Button buttonPgDn;
        Splitter splitterIndicator;

    public:
        ItemsScroll()
        = default;
    };
}

