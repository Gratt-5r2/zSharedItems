// Supported with union (c) 2020 Union team

// User API for oCNpcInventory
// Add your methods here

void PackItemsInCategory_Union( int packEquipped );
void UnpackItemsInCategory_Union();
int PackSingleItem_Union( oCItem* item );
int GetPackedItemInfo_Union( const zSTRING& name, int destroyEntry, int& amount, int& __equipped );
oCItem* CreateFromPackString_Union( const zSTRING& name );