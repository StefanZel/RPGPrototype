#pragma once

#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
namespace EntityTags
{
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Root_Stat);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Root_Condition);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Root_Damage);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Root_Ability);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Root_State);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Root_Modifier);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Root_Resource);
    
    
    namespace Stat
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Defense);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Offense);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Resource);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
        
        namespace Attributes
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Strength);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dexterity);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Intelligence);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Constitution);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Wisdom);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Charisma);
        }
        
        namespace Defenses
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Armor);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(MagicResist);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireResist);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(IceResist);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(LightningResist);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(PoisonResist);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(DodgeChance);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlockChance);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(ParryChance);
        }
        
        namespace Offenses
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(AttackDamage);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpellDamage);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(CriticalChance);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(CriticalMultiplier);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Accuracy);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Penetration);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(AttackSpeed);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(CastSpeed);
        }
        
        namespace Resources
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Health);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(HealthMax);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(HealthCurrent);
            
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Mana);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(ManaMax);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(ManaCurrent);
            
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stamina);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(StaminaMax);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(StaminaCurrent);
            
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(EnergyShield);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(EnergyShieldMax);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(EnergyShieldCurrent);
        }
        
        namespace Secondaries
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(MovementSpeed);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Initiative);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(CooldownReduction);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(ExperienceGain);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(GoldFind);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(ItemRarity);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Level);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Experience);
        }
    }
    
    namespace Condition
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(BattleActive);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(BattleTurnActive);
        
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentWeapon);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentWeaponSword);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentWeaponBow);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentWeaponStaff);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentArmor);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentShield);
        
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(State);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateHealthLow);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateHealthCritical);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateManaLow);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateBuffed);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateDebuffed);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateCrowdControlled);
    }
    
    namespace Damage
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Physical);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Magic);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Fire);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ice);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lightning);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Poison);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Holy);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dark);
    }
    
   namespace Ability
    {
        namespace Types
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Melee);   
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ranged);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Placeable);
        }
        namespace Uses
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Heal);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Buff);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Aura);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Curse);
        }
        
    }
   
    namespace State
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Alive);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dead);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stunned);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Silenced);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Rooted);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Invulnerable);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Invisible);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stealthed);
    }
   
    namespace Modifier
    {
        namespace Source
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Passive);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Buff);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Scaling);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Environment);
        }
    }
    
    void InitializeResourceMappings();
    FGameplayTag GetResourceMaxTag(const FGameplayTag& ResourceCurrentTag);
    FGameplayTag GetResourceCurrentTag(const FGameplayTag& ResourceMaxTag);
    FGameplayTag GetResourceBaseTag(const FGameplayTag& ResourceTag);
    FGameplayTag GetResourceMaxTagFromBase(const FGameplayTag& ResourceTag);
    FGameplayTag GetResourceCurrentTagFromBase(const FGameplayTag& ResourceTag);
    bool IsResourceMaxTag(const FGameplayTag& Tag);
    bool IsResourceCurrentTag(const FGameplayTag& Tag);
    bool IsResourceBaseTag(const FGameplayTag& Tag);
    
    
    bool IsStatTag(const FGameplayTag& Tag);
    bool IsResourceTag(const FGameplayTag& Tag);
    bool IsDamageTag(const FGameplayTag& Tag);
    
    FGameplayTag GetStatCategory(const FGameplayTag& Tag);
    
    FGameplayTag GetResourceMaxTag(const FGameplayTag& ResourceCurrentTag);
    FGameplayTag GetResourceCurrentTag(const FGameplayTag& ResourceMaxTag);
    
    const FGameplayTagContainer& GetAllAttributeTags();
    const FGameplayTagContainer& GetAllDefenseTags();
    const FGameplayTagContainer& GetAllOffenseTags();
    const FGameplayTagContainer& GetAllResourceTags();
    const FGameplayTagContainer& GetPhysicalDamageTags();
}