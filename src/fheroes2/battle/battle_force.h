//
// Created by Ciprian Khlud on 2019-04-26.
//

#pragma once

#include "battle_army.h"

namespace Battle
{
    class Force : public Units, public BitModes
    {
    public:
        Force(Army&, bool);

        ~Force();

        HeroBase *GetCommander();

        const HeroBase *GetCommander() const;

        bool isValid() const;

        bool HasMonster(const Monster&) const;

        uint32_t GetDeadHitPoints() const;

        uint32_t GetDeadCounts() const;

        int GetColor() const;

        int GetControl() const;

        uint32_t GetSurrenderCost() const;

        Troops GetKilledTroops() const;

        bool SetIdleAnimation();

        bool NextIdleAnimation();

        void NewTurn();

        void SyncArmyCount();

        static Unit *GetCurrentUnit(const Force&, const Force&, Unit *last, Units *all, bool part1);

        static Unit *GetCurrentUnit(const Force&, const Force&, Unit *last, bool part1);

        static void UpdateOrderUnits(const Force&, const Force&, Units&);

    private:
        Army& army;
        vector <uint32_t> uids;
    };

    ByteVectorWriter& operator<<(ByteVectorWriter&, const Force&);

    ByteVectorReader& operator>>(ByteVectorReader&, Force&);
}
