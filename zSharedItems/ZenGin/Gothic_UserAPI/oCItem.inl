// Supported with union (c) 2020 Union team

// User API for oCItem
// Add your methods here

void Archive_Union( zCArchiver& );
void Unarchive_Union( zCArchiver& );
bool IsHeroOwner() { return (string&)objectName == "HERO"; };
zSTRING GetInstanceName_Union();

bool IsEquiped() {
  return HasFlag( ITM_FLAG_ACTIVE ) != 0;
}

bool IsWeapon() {
  return (mainflag & ITM_CAT_NF || mainflag & ITM_CAT_FF) != 0;
}

bool IsArmor() {
  return (mainflag & ITM_CAT_ARMOR) != 0;
}

bool IsMagic() {
  return (mainflag & ITM_CAT_MAGIC) != 0;
}