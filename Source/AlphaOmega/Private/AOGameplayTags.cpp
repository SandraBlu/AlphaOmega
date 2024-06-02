// Fill out your copyright notice in the Description page of Project Settings.


#include "AOGameplayTags.h"
#include "GameplayTagsManager.h"

FAOGameplayTags FAOGameplayTags::GameplayTags;


void FAOGameplayTags::InitializeNativeGameplayTags()
{
	//primary attributes
	GameplayTags.Attributes_Primary_Agility = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Agility"), FString("Improves dodge and stealth skills"));
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Intelligence"), FString("Improves Energy tech damage"));
	GameplayTags.Attributes_Primary_Perception = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Perception"), FString("Allows you to see AI better, improves stealth"));
	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Resilience"), FString("Increases Armor and Armor Penetration"));
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Strength"), FString("Increases  physical damage"));
	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Vigor"), FString("Increases health"));

	//secondary attributes
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armor"), FString("Reduces health damage"));
	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.ArmorPenetration"), FString("Increases damage to enemy"));
	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.BlockChance"), FString("Decreases damage to self"));
	GameplayTags.Attributes_Secondary_DodgeChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.DodgeChance"), FString("Negates damage"));
	GameplayTags.Attributes_Secondary_Stealth = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Stealth"), FString("Negates AI perception"));
	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitChance"), FString("Increases damage to enemy"));
	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitDamage"), FString("Increases hit damage to enemy"));
	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitResistance"), FString("Decreases damage to self"));
	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxHealth"), FString("Clamps Health"));
	GameplayTags.Attributes_Secondary_MaxStamina = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxStamina"), FString("Clamps Stamina"));
	GameplayTags.Attributes_Secondary_MaxEnergy = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxEnergy"), FString("Clamps Energy"));
	GameplayTags.Attributes_Secondary_HealthRegen = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.HealthRegen"), FString("Rate of Health Regeneration"));
	GameplayTags.Attributes_Secondary_StaminaRegen = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.StaminaRegen"), FString("Rate of Stamina Regeneration"));
	GameplayTags.Attributes_Secondary_EnergyRegen = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.EnergyRegen"), FString("Rate of Energy Regeneration"));

	GameplayTags.Attributes_meta_xp = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.meta.xp"), FString("XP Meta Attribute"));

	//Input Tags
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.LMB"), FString("LMB"));
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.RMB"), FString("RMB"));
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.1"), FString("1"));
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.2"), FString("2"));
	GameplayTags.InputTag_Passive01 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Passive01"), FString("Passive01"));
	GameplayTags.InputTag_Passive02 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Passive02"), FString("Passive02"));
	GameplayTags.InputTag_Passive03 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Passive02"), FString("Passive03"));

	//Damage Meta Att
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage"), FString("Damage"));
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Physical"), FString("Physical Damage"));
	GameplayTags.Damage_Poison = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Poison"), FString("Poison Damage"));
	GameplayTags.Damage_Elemental_Dark = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Elemental.Dark"), FString("Dark Damage"));
	GameplayTags.Damage_Elemental_Electric = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Elemental.Electric"), FString("Electric Damage"));
	GameplayTags.Damage_Elemental_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Elemental.Fire"), FString("Fire Damage"));
	GameplayTags.Damage_Elemental_Ice = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Elemental.Ice"), FString("Ice Damage"));


	//Resistance Tags
	GameplayTags.Attributes_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Physical"), FString("Resistance to Physical damage"));
	GameplayTags.Attributes_Resistance_Poison = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Poison"), FString("Resistance to Poison damage"));
	GameplayTags.Attributes_Resistance_Dark = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Dark"), FString("Resistance to Dark damage"));
	GameplayTags.Attributes_Resistance_Electric = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Electric"), FString("Resistance to Electric damage"));
	GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Fire"), FString("Resistance to Fire damage"));
	GameplayTags.Attributes_Resistance_Ice = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Ice"), FString("Resistance to Cold damage"));

	//De-buffs
	GameplayTags.Debuff_KnockOut = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("debuff.knockout"), FString("De-buff to Physical damage"));
	GameplayTags.Debuff_Paralyze = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("debuff.paralyze"), FString("De-buff to Poison"));
	GameplayTags.Debuff_Dark = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("debuff.dark"), FString("De-buff to Dark Damage"));
	GameplayTags.Debuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("debuff.stun"), FString("De-buff to Electric damage"));
	GameplayTags.Debuff_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("debuff.burn"), FString("De-buff to fire damage"));
	GameplayTags.Debuff_Freeze = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("debuff.freeze"), FString("De-buff to ice damage"));

	GameplayTags.Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("debuff.damage"), FString("De-buff  Damage"));
	GameplayTags.Debuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("debuff.chance"), FString("De-buff chance"));
	GameplayTags.Debuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("debuff.frequency"), FString("De-buff Frequency"));
	GameplayTags.Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("debuff.duration"), FString("De-buff duration"));

	// 
	//Ability
	GameplayTags.ability_none = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.none"), FString("No Ability"));
	GameplayTags.ability_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.Attack"), FString("General attack tag for Behavior Tree"));
	GameplayTags.ability_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.Melee"), FString("Melee Attack tag"));
	GameplayTags.ability_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.HitReact"), FString("Hit React tag"));
	GameplayTags.ability_missile_dark = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.missile.dark"), FString("Dark Missile tag"));
	GameplayTags.ability_missile_electric = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.missile.electric"), FString("Electric Missile tag"));
	GameplayTags.ability_missile_fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.missile.fire"), FString("Fire Missile tag"));
	GameplayTags.ability_missile_ice = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.missile.ice"), FString("Ice Missile tag"));
	GameplayTags.ability_dart_poison = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.dart.poison"), FString("Poison Dart tag"));
	GameplayTags.ability_dart_knockout = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.dart.knockout"), FString("Knockout Dart tag"));

	//Ability Menu Tags
	GameplayTags.ability_status_locked = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.status.locked"), FString("Locked Status"));;
	GameplayTags.ability_status_unlocked = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.status.unlocked"), FString("Unlocked Status"));;
	GameplayTags.ability_status_available = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.status.available"), FString("Available Status"));;
	GameplayTags.ability_status_active = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.status.active"), FString("Active Status"));;

	GameplayTags.ability_type_dynamic = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.type.dynamic"), FString("Dynamic"));;
	GameplayTags.ability_type_passive = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.type.passive"), FString("Passive"));;
	GameplayTags.ability_type_none = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("ability.type.none"), FString("None"));;

	//cooldown//
	GameplayTags.cooldown_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("cooldown.melee"), FString("Melee cooldown tag"));
	GameplayTags.cooldown_missile_dark = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("cooldown.missile.dark"), FString("Dark Missile cooldown tag"));
	GameplayTags.cooldown_missile_electric = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("cooldown.missile.electric"), FString("Electric Missile cooldown tag"));
	GameplayTags.cooldown_missile_fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("cooldown.missile.fire"), FString("Fire Missile cooldown tag"));
	GameplayTags.cooldown_missile_ice = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("cooldown.missile.ice"), FString("Ice Missile cooldown tag"));
	GameplayTags.cooldown_dart_poison = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("cooldown.dart.poison"), FString("Poison Dart cooldown tag"));
	GameplayTags.cooldown_dart_knockout = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("cooldown.dart.knockout"), FString("Knockout Dart cooldown tag"));

	//Sockets
	GameplayTags.combatSocket_weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("combatSocket.weapon"), FString("weapon"));
	GameplayTags.combatSocket_handL = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("combatSocket.handR"), FString("handR"));
	GameplayTags.combatSocket_handR = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("combatSocket.handL"), FString("handL"));

	//Montages
	GameplayTags.Montage_Attack01 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("montage.attack01"), FString("attack01"));
	GameplayTags.Montage_Attack02 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("montage.attack02"), FString("attack02"));
	GameplayTags.Montage_Attack03 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("montage.attack03"), FString("attack03"));
	GameplayTags.Montage_Attack04 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("montage.attack04"), FString("attack04"));

	/*
	 * Map of Damage Types to Resistances
	 */
	GameplayTags.DamageTypeToResist.Add(GameplayTags.Damage_Physical, GameplayTags.Attributes_Resistance_Physical);
	GameplayTags.DamageTypeToResist.Add(GameplayTags.Damage_Poison, GameplayTags.Attributes_Resistance_Poison);
	GameplayTags.DamageTypeToResist.Add(GameplayTags.Damage_Elemental_Dark, GameplayTags.Attributes_Resistance_Dark);
	GameplayTags.DamageTypeToResist.Add(GameplayTags.Damage_Elemental_Electric, GameplayTags.Attributes_Resistance_Electric);
	GameplayTags.DamageTypeToResist.Add(GameplayTags.Damage_Elemental_Fire, GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypeToResist.Add(GameplayTags.Damage_Elemental_Ice, GameplayTags.Attributes_Resistance_Ice);

	/*
	 * Map of Damage Types to De-buffs
	 */
	GameplayTags.DamageTypeToDebuff.Add(GameplayTags.Damage_Physical, GameplayTags.Debuff_KnockOut);
	GameplayTags.DamageTypeToDebuff.Add(GameplayTags.Damage_Poison, GameplayTags.Debuff_Paralyze);
	GameplayTags.DamageTypeToDebuff.Add(GameplayTags.Damage_Elemental_Dark, GameplayTags.Debuff_Dark);
	GameplayTags.DamageTypeToDebuff.Add(GameplayTags.Damage_Elemental_Electric, GameplayTags.Debuff_Stun);
	GameplayTags.DamageTypeToDebuff.Add(GameplayTags.Damage_Elemental_Fire, GameplayTags.Debuff_Burn);
	GameplayTags.DamageTypeToDebuff.Add(GameplayTags.Damage_Elemental_Ice, GameplayTags.Debuff_Freeze);
	
}