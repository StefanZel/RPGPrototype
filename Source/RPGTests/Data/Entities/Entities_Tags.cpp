#include "Entities_Tags.h"

namespace EntityTags
{

    UE_DEFINE_GAMEPLAY_TAG(Root_Stat, "Stat");
    UE_DEFINE_GAMEPLAY_TAG(Root_Condition, "Condition");
    UE_DEFINE_GAMEPLAY_TAG(Root_Damage, "Damage");
    UE_DEFINE_GAMEPLAY_TAG(Root_Ability, "Ability");
    UE_DEFINE_GAMEPLAY_TAG(Root_State, "State");
    UE_DEFINE_GAMEPLAY_TAG(Root_Modifier, "Modifier");
    UE_DEFINE_GAMEPLAY_TAG(Root_Resource, "Resource");
 
    namespace Stat
    {
        UE_DEFINE_GAMEPLAY_TAG(Attribute, "Stat.Attribute");
        UE_DEFINE_GAMEPLAY_TAG(Defense, "Stat.Defense");
        UE_DEFINE_GAMEPLAY_TAG(Offense, "Stat.Offense");
        UE_DEFINE_GAMEPLAY_TAG(Resource, "Stat.Resource");
        UE_DEFINE_GAMEPLAY_TAG(Secondary, "Stat.Secondary");
        
        namespace Attributes
        {
            UE_DEFINE_GAMEPLAY_TAG(Strength, "Stat.Attribute.Strength");
            UE_DEFINE_GAMEPLAY_TAG(Dexterity, "Stat.Attribute.Dexterity");
            UE_DEFINE_GAMEPLAY_TAG(Intelligence, "Stat.Attribute.Intelligence");
            UE_DEFINE_GAMEPLAY_TAG(Constitution, "Stat.Attribute.Constitution");
            UE_DEFINE_GAMEPLAY_TAG(Wisdom, "Stat.Attribute.Wisdom");
            UE_DEFINE_GAMEPLAY_TAG(Charisma, "Stat.Attribute.Charisma");
        }
        
        namespace Defenses
        {
            UE_DEFINE_GAMEPLAY_TAG(Armor, "Stat.Defense.Armor");
            UE_DEFINE_GAMEPLAY_TAG(MagicResist, "Stat.Defense.MagicResist");
            UE_DEFINE_GAMEPLAY_TAG(FireResist, "Stat.Defense.FireResist");
            UE_DEFINE_GAMEPLAY_TAG(IceResist, "Stat.Defense.IceResist");
            UE_DEFINE_GAMEPLAY_TAG(LightningResist, "Stat.Defense.LightningResist");
            UE_DEFINE_GAMEPLAY_TAG(PoisonResist, "Stat.Defense.PoisonResist");
            UE_DEFINE_GAMEPLAY_TAG(DodgeChance, "Stat.Defense.DodgeChance");
            UE_DEFINE_GAMEPLAY_TAG(BlockChance, "Stat.Defense.BlockChance");
            UE_DEFINE_GAMEPLAY_TAG(ParryChance, "Stat.Defense.ParryChance");
        }
        
        namespace Offenses
        {
            UE_DEFINE_GAMEPLAY_TAG(AttackDamage, "Stat.Offense.AttackDamage");
            UE_DEFINE_GAMEPLAY_TAG(SpellDamage, "Stat.Offense.SpellDamage");
            UE_DEFINE_GAMEPLAY_TAG(CriticalChance, "Stat.Offense.CriticalChance");
            UE_DEFINE_GAMEPLAY_TAG(CriticalMultiplier, "Stat.Offense.CriticalMultiplier");
            UE_DEFINE_GAMEPLAY_TAG(Accuracy, "Stat.Offense.Accuracy");
            UE_DEFINE_GAMEPLAY_TAG(Penetration, "Stat.Offense.Penetration");
            UE_DEFINE_GAMEPLAY_TAG(AttackSpeed, "Stat.Offense.AttackSpeed");
            UE_DEFINE_GAMEPLAY_TAG(CastSpeed, "Stat.Offense.CastSpeed");
        }
        
        namespace Resources
        {
            UE_DEFINE_GAMEPLAY_TAG(Health, "Stat.Resource.Health");
            UE_DEFINE_GAMEPLAY_TAG(HealthMax, "Stat.Resource.Health.Max");
            UE_DEFINE_GAMEPLAY_TAG(HealthCurrent, "Stat.Resource.Health.Current");
            
            UE_DEFINE_GAMEPLAY_TAG(Mana, "Stat.Resource.Mana");
            UE_DEFINE_GAMEPLAY_TAG(ManaMax, "Stat.Resource.Mana.Max");
            UE_DEFINE_GAMEPLAY_TAG(ManaCurrent, "Stat.Resource.Mana.Current");
            
            UE_DEFINE_GAMEPLAY_TAG(Stamina, "Stat.Resource.Stamina");
            UE_DEFINE_GAMEPLAY_TAG(StaminaMax, "Stat.Resource.Stamina.Max");
            UE_DEFINE_GAMEPLAY_TAG(StaminaCurrent, "Stat.Resource.Stamina.Current");
            
            UE_DEFINE_GAMEPLAY_TAG(EnergyShield, "Stat.Resource.EnergyShield");
            UE_DEFINE_GAMEPLAY_TAG(EnergyShieldMax, "Stat.Resource.EnergyShield.Max");
            UE_DEFINE_GAMEPLAY_TAG(EnergyShieldCurrent, "Stat.Resource.EnergyShield.Current");
        }
        
        namespace Secondaries
        {
            UE_DEFINE_GAMEPLAY_TAG(MovementSpeed, "Stat.Secondary.MovementSpeed");
            UE_DEFINE_GAMEPLAY_TAG(Initiative, "Stat.Secondary.Initiative");
            UE_DEFINE_GAMEPLAY_TAG(CooldownReduction, "Stat.Secondary.CooldownReduction");
            UE_DEFINE_GAMEPLAY_TAG(ExperienceGain, "Stat.Secondary.ExperienceGain");
            UE_DEFINE_GAMEPLAY_TAG(GoldFind, "Stat.Secondary.GoldFind");
            UE_DEFINE_GAMEPLAY_TAG(ItemRarity, "Stat.Secondary.ItemRarity");
            UE_DEFINE_GAMEPLAY_TAG(Level, "Stat.Secondary.Level");
            UE_DEFINE_GAMEPLAY_TAG(Experience, "Stat.Secondary.Experience");
        }
    }
    
    namespace Condition
    {
        UE_DEFINE_GAMEPLAY_TAG(Battle, "Condition.Battle");
        UE_DEFINE_GAMEPLAY_TAG(BattleActive, "Condition.Battle.Active");
        UE_DEFINE_GAMEPLAY_TAG(BattleTurnActive, "Condition.Battle.TurnActive");
        
        UE_DEFINE_GAMEPLAY_TAG(Equipment, "Condition.Equipment");
        UE_DEFINE_GAMEPLAY_TAG(EquipmentWeapon, "Condition.Equipment.Weapon");
        UE_DEFINE_GAMEPLAY_TAG(EquipmentWeaponSword, "Condition.Equipment.Weapon.Sword");
        UE_DEFINE_GAMEPLAY_TAG(EquipmentWeaponBow, "Condition.Equipment.Weapon.Bow");
        UE_DEFINE_GAMEPLAY_TAG(EquipmentWeaponStaff, "Condition.Equipment.Weapon.Staff");
        UE_DEFINE_GAMEPLAY_TAG(EquipmentArmor, "Condition.Equipment.Armor");
        UE_DEFINE_GAMEPLAY_TAG(EquipmentShield, "Condition.Equipment.Shield");
        
        UE_DEFINE_GAMEPLAY_TAG(State, "Condition.State");
        UE_DEFINE_GAMEPLAY_TAG(StateHealthLow, "Condition.State.HealthLow");
        UE_DEFINE_GAMEPLAY_TAG(StateHealthCritical, "Condition.State.HealthCritical");
        UE_DEFINE_GAMEPLAY_TAG(StateManaLow, "Condition.State.ManaLow");
        UE_DEFINE_GAMEPLAY_TAG(StateBuffed, "Condition.State.Buffed");
        UE_DEFINE_GAMEPLAY_TAG(StateDebuffed, "Condition.State.Debuffed");
        UE_DEFINE_GAMEPLAY_TAG(StateCrowdControlled, "Condition.State.CrowdControlled");
    }
    
    namespace Damage
    {
        UE_DEFINE_GAMEPLAY_TAG(Physical, "Damage.Physical");
        UE_DEFINE_GAMEPLAY_TAG(Magic, "Damage.Magic");
        UE_DEFINE_GAMEPLAY_TAG(Fire, "Damage.Fire");
        UE_DEFINE_GAMEPLAY_TAG(Ice, "Damage.Ice");
        UE_DEFINE_GAMEPLAY_TAG(Lightning, "Damage.Lightning");
        UE_DEFINE_GAMEPLAY_TAG(Poison, "Damage.Poison");
        UE_DEFINE_GAMEPLAY_TAG(Holy, "Damage.Holy");
        UE_DEFINE_GAMEPLAY_TAG(Dark, "Damage.Dark");
    }
    
    namespace Ability
    {
        UE_DEFINE_GAMEPLAY_TAG(Type, "Ability.Type");
        UE_DEFINE_GAMEPLAY_TAG(TypePhysical, "Ability.Type.Physical");
        UE_DEFINE_GAMEPLAY_TAG(TypeMagic, "Ability.Type.Magic");
        UE_DEFINE_GAMEPLAY_TAG(TypeFire, "Ability.Type.Fire");
        UE_DEFINE_GAMEPLAY_TAG(TypeIce, "Ability.Type.Ice");
        UE_DEFINE_GAMEPLAY_TAG(TypeHealing, "Ability.Type.Healing");
        UE_DEFINE_GAMEPLAY_TAG(TypeBuff, "Ability.Type.Buff");
        UE_DEFINE_GAMEPLAY_TAG(TypeDebuff, "Ability.Type.Debuff");
        UE_DEFINE_GAMEPLAY_TAG(TypeAura, "Ability.Type.Aura");
        UE_DEFINE_GAMEPLAY_TAG(TypeCurse, "Ability.Type.Curse");
    }
 
    namespace State
    {
        UE_DEFINE_GAMEPLAY_TAG(Alive, "State.Alive");
        UE_DEFINE_GAMEPLAY_TAG(Dead, "State.Dead");
        UE_DEFINE_GAMEPLAY_TAG(Stunned, "State.Stunned");
        UE_DEFINE_GAMEPLAY_TAG(Silenced, "State.Silenced");
        UE_DEFINE_GAMEPLAY_TAG(Rooted, "State.Rooted");
        UE_DEFINE_GAMEPLAY_TAG(Invulnerable, "State.Invulnerable");
        UE_DEFINE_GAMEPLAY_TAG(Invisible, "State.Invisible");
        UE_DEFINE_GAMEPLAY_TAG(Stealthed, "State.Stealthed");
    }
 
    namespace Modifier
    {
        namespace Source
        {
            UE_DEFINE_GAMEPLAY_TAG(Item, "Modifier.Source.Item");
            UE_DEFINE_GAMEPLAY_TAG(Passive, "Modifier.Source.Passive");
            UE_DEFINE_GAMEPLAY_TAG(Ability, "Modifier.Source.Ability");
            UE_DEFINE_GAMEPLAY_TAG(Buff, "Modifier.Source.Buff");
            UE_DEFINE_GAMEPLAY_TAG(Debuff, "Modifier.Source.Debuff");
            UE_DEFINE_GAMEPLAY_TAG(Scaling, "Modifier.Source.Scaling");
            UE_DEFINE_GAMEPLAY_TAG(Environment, "Modifier.Source.Environment");
        }
    }
    namespace Private
    {
        static bool bResourceMappingsInitialized = false;
        
        void EnsureResourceMappings()
        {
            if (!bResourceMappingsInitialized)
            {
                InitializeResourceMappings();
                bResourceMappingsInitialized = true;
            }
        }
        
        static TMap<FGameplayTag, FGameplayTag> ResourceMaxToCurrent;
        static TMap<FGameplayTag, FGameplayTag> ResourceCurrentToMax;
        static TMap<FGameplayTag, FGameplayTag> ResourceVariantToBase;
        static TMap<FGameplayTag, FGameplayTag> ResourceBaseToMax;
        static TMap<FGameplayTag, FGameplayTag> ResourceBaseToCurrent;
        
        static TOptional<FGameplayTagContainer> AllResourceMaxTags;
        static TOptional<FGameplayTagContainer> AllResourceCurrentTags;
        static TOptional<FGameplayTagContainer> AllResourceBaseTags;
        
        void InitializeResourceMappings()
        {
            ResourceMaxToCurrent.Add(Stat::Resources::HealthMax, Stat::Resources::HealthCurrent);
            ResourceCurrentToMax.Add(Stat::Resources::HealthCurrent, Stat::Resources::HealthMax);
            ResourceVariantToBase.Add(Stat::Resources::HealthMax, Stat::Resources::Health);
            ResourceVariantToBase.Add(Stat::Resources::HealthCurrent, Stat::Resources::Health);
            ResourceBaseToMax.Add(Stat::Resources::Health, Stat::Resources::HealthMax);
            ResourceBaseToCurrent.Add(Stat::Resources::Health, Stat::Resources::HealthCurrent);

            ResourceMaxToCurrent.Add(Stat::Resources::ManaMax, Stat::Resources::ManaCurrent);
            ResourceCurrentToMax.Add(Stat::Resources::ManaCurrent, Stat::Resources::ManaMax);
            ResourceVariantToBase.Add(Stat::Resources::ManaMax, Stat::Resources::Mana);
            ResourceVariantToBase.Add(Stat::Resources::ManaCurrent, Stat::Resources::Mana);
            ResourceBaseToMax.Add(Stat::Resources::Mana, Stat::Resources::ManaMax);
            ResourceBaseToCurrent.Add(Stat::Resources::Mana, Stat::Resources::ManaCurrent);

            ResourceMaxToCurrent.Add(Stat::Resources::StaminaMax, Stat::Resources::StaminaCurrent);
            ResourceCurrentToMax.Add(Stat::Resources::StaminaCurrent, Stat::Resources::StaminaMax);
            ResourceVariantToBase.Add(Stat::Resources::StaminaMax, Stat::Resources::Stamina);
            ResourceVariantToBase.Add(Stat::Resources::StaminaCurrent, Stat::Resources::Stamina);
            ResourceBaseToMax.Add(Stat::Resources::Stamina, Stat::Resources::StaminaMax);
            ResourceBaseToCurrent.Add(Stat::Resources::Stamina, Stat::Resources::StaminaCurrent);

            ResourceMaxToCurrent.Add(Stat::Resources::EnergyShieldMax, Stat::Resources::EnergyShieldCurrent);
            ResourceCurrentToMax.Add(Stat::Resources::EnergyShieldCurrent, Stat::Resources::EnergyShieldMax);
            ResourceVariantToBase.Add(Stat::Resources::EnergyShieldMax, Stat::Resources::EnergyShield);
            ResourceVariantToBase.Add(Stat::Resources::EnergyShieldCurrent, Stat::Resources::EnergyShield);
            ResourceBaseToMax.Add(Stat::Resources::EnergyShield, Stat::Resources::EnergyShieldMax);
            ResourceBaseToCurrent.Add(Stat::Resources::EnergyShield, Stat::Resources::EnergyShieldCurrent);
            
            AllResourceMaxTags.Emplace();
            AllResourceCurrentTags.Emplace();
            AllResourceBaseTags.Emplace();
            for (const auto& Pair : ResourceMaxToCurrent)
            {
                AllResourceMaxTags->AddTag(Pair.Key);
                AllResourceCurrentTags->AddTag(Pair.Value);
            }
            for (const auto& Pair : ResourceBaseToMax)
            {
                AllResourceBaseTags->AddTag(Pair.Key);
            }
        }
    }
    void InitializeResourceMappings()
    {
        Private::InitializeResourceMappings();
    }
    
    FGameplayTag GetResourceMaxTag(const FGameplayTag& ResourceCurrentTag)
    {
        Private::EnsureResourceMappings();
        return Private::ResourceCurrentToMax.FindRef(ResourceCurrentTag);
    }
    
    FGameplayTag GetResourceCurrentTag(const FGameplayTag& ResourceMaxTag)
    {
        Private::EnsureResourceMappings();
        return Private::ResourceMaxToCurrent.FindRef(ResourceMaxTag);
    }
    
    FGameplayTag GetResourceBaseTag(const FGameplayTag& ResourceVariantTag)
    {
        Private::EnsureResourceMappings();
        return Private::ResourceVariantToBase.FindRef(ResourceVariantTag);
    }

    FGameplayTag GetResourceMaxTagFromBase(const FGameplayTag& ResourceTag)
    {
        Private::EnsureResourceMappings();
        return Private::ResourceBaseToMax.FindRef(ResourceTag);
    }

    FGameplayTag GetResourceCurrentTagFromBase(const FGameplayTag& ResourceTag)
    {
        Private::EnsureResourceMappings();
        return Private::ResourceBaseToCurrent.FindRef(ResourceTag);
    }

    bool IsResourceMaxTag(const FGameplayTag& Tag)
    {
        Private::EnsureResourceMappings();
        return Private::ResourceMaxToCurrent.Contains(Tag);
    }
    
    bool IsResourceCurrentTag(const FGameplayTag& Tag)
    {
        Private::EnsureResourceMappings();
        return Private::ResourceCurrentToMax.Contains(Tag);
    }

    bool IsResourceBaseTag(const FGameplayTag& Tag)
    {
        Private::EnsureResourceMappings();
        return Private::ResourceBaseToMax.Contains(Tag);
    }

    namespace Private
    {
        static TOptional<FGameplayTagContainer> GAllAttributeTags;
        static TOptional<FGameplayTagContainer> GAllDefenseTags;
        static TOptional<FGameplayTagContainer> GAllOffenseTags;
        static TOptional<FGameplayTagContainer> GAllResourceTags;
        static TOptional<FGameplayTagContainer> GPhysicalDamageTags;
        
        void InitializeContainers()
        {
            if (!GAllAttributeTags.IsSet())
            {
                GAllAttributeTags.Emplace();
                GAllAttributeTags->AddTag(Stat::Attributes::Strength);
                GAllAttributeTags->AddTag(Stat::Attributes::Dexterity);
                GAllAttributeTags->AddTag(Stat::Attributes::Intelligence);
                GAllAttributeTags->AddTag(Stat::Attributes::Constitution);
                GAllAttributeTags->AddTag(Stat::Attributes::Wisdom);
                GAllAttributeTags->AddTag(Stat::Attributes::Charisma);
            }
            
            if (!GAllDefenseTags.IsSet())
            {
                GAllDefenseTags.Emplace();
                GAllDefenseTags->AddTag(Stat::Defenses::Armor);
                GAllDefenseTags->AddTag(Stat::Defenses::MagicResist);
                GAllDefenseTags->AddTag(Stat::Defenses::FireResist);
                GAllDefenseTags->AddTag(Stat::Defenses::IceResist);
                GAllDefenseTags->AddTag(Stat::Defenses::LightningResist);
                GAllDefenseTags->AddTag(Stat::Defenses::PoisonResist);
                GAllDefenseTags->AddTag(Stat::Defenses::DodgeChance);
                GAllDefenseTags->AddTag(Stat::Defenses::BlockChance);
                GAllDefenseTags->AddTag(Stat::Defenses::ParryChance);
            }
            
            if (!GAllOffenseTags.IsSet())
            {
                GAllOffenseTags.Emplace();
                GAllOffenseTags->AddTag(Stat::Offenses::AttackDamage);
                GAllOffenseTags->AddTag(Stat::Offenses::SpellDamage);
                GAllOffenseTags->AddTag(Stat::Offenses::CriticalChance);
                GAllOffenseTags->AddTag(Stat::Offenses::CriticalMultiplier);
                GAllOffenseTags->AddTag(Stat::Offenses::Accuracy);
                GAllOffenseTags->AddTag(Stat::Offenses::Penetration);
                GAllOffenseTags->AddTag(Stat::Offenses::AttackSpeed);
                GAllOffenseTags->AddTag(Stat::Offenses::CastSpeed);
            }
            
            if (!GAllResourceTags.IsSet())
            {
                GAllResourceTags.Emplace();
                GAllResourceTags->AddTag(Stat::Resources::HealthMax);
                GAllResourceTags->AddTag(Stat::Resources::HealthCurrent);
                GAllResourceTags->AddTag(Stat::Resources::ManaMax);
                GAllResourceTags->AddTag(Stat::Resources::ManaCurrent);
                GAllResourceTags->AddTag(Stat::Resources::StaminaMax);
                GAllResourceTags->AddTag(Stat::Resources::StaminaCurrent);
                GAllResourceTags->AddTag(Stat::Resources::EnergyShieldMax);
                GAllResourceTags->AddTag(Stat::Resources::EnergyShieldCurrent);
            }
            
            if (!GPhysicalDamageTags.IsSet())
            {
                GPhysicalDamageTags.Emplace();
                GPhysicalDamageTags->AddTag(Damage::Physical);
            }
        }
    }
 
    bool IsStatTag(const FGameplayTag& Tag)
    {
        return Tag.MatchesTag(Root_Stat);
    }
    
    bool IsResourceTag(const FGameplayTag& Tag)
    {
        return Tag.MatchesTag(Stat::Resource);
    }
    
    bool IsDamageTag(const FGameplayTag& Tag)
    {
        return Tag.MatchesTag(Root_Damage);
    }
    
    FGameplayTag GetStatCategory(const FGameplayTag& Tag)
    {
        if (Tag.MatchesTag(Stat::Attribute)) return Stat::Attribute;
        if (Tag.MatchesTag(Stat::Defense)) return Stat::Defense;
        if (Tag.MatchesTag(Stat::Offense)) return Stat::Offense;
        if (Tag.MatchesTag(Stat::Resource)) return Stat::Resource;
        if (Tag.MatchesTag(Stat::Secondary)) return Stat::Secondary;
        
        return FGameplayTag::EmptyTag;
    }
    
    const FGameplayTagContainer& GetAllAttributeTags()
    {
        Private::InitializeContainers();
        return Private::GAllAttributeTags.GetValue();
    }
    
    const FGameplayTagContainer& GetAllDefenseTags()
    {
        Private::InitializeContainers();
        return Private::GAllDefenseTags.GetValue();
    }
    
    const FGameplayTagContainer& GetAllOffenseTags()
    {
        Private::InitializeContainers();
        return Private::GAllOffenseTags.GetValue();
    }
    
    const FGameplayTagContainer& GetAllResourceTags()
    {
        Private::InitializeContainers();
        return Private::GAllResourceTags.GetValue();
    }
    
    const FGameplayTagContainer& GetPhysicalDamageTags()
    {
        Private::InitializeContainers();
        return Private::GPhysicalDamageTags.GetValue();
    }
}