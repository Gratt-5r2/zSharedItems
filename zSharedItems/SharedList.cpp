// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  void zTSharedList::Insert( oCItem* item, int amount ) {
    string instanceName = item->GetInstanceName();
    auto& pair = Items[instanceName];

    if( pair.IsNull() )
      return Items.Insert( instanceName, amount );

    pair.GetValue() += amount;
  }



  void zTSharedList::Remove( oCItem* item, int amount ) {
    string instanceName = item->GetInstanceName();
    auto& pair = Items[instanceName];

    if( pair.IsNull() )
      return;

    int& value = pair;
    if( amount >= value ) {
      Items.Remove( instanceName );
      return;
    }

    value -= amount;
  }



  bool zTSharedList::operator == ( const string& other ) const {
    return NpcInstance == other;
  }



  bool zTSharedList::operator > ( const string& other ) const {
    return NpcInstance > other;
  }



  bool zTSharedList::operator < ( const string& other ) const {
    return NpcInstance < other;
  }



  zTSharedList::operator const string& () const {
    return NpcInstance;
  }



  void zTSharedList::Archive( zCArchiver& ar ) {
    ar.WriteString( "Instance", Z NpcInstance );
    ar.WriteInt( "ItemsCount", Items.GetNum() );

    auto& array = Items.GetArray();
    for( uint i = 0; i < array.GetNum(); i++ ) {
      ar.WriteString( "Item", array[i].GetKey() );
      ar.WriteInt( "Amount", array[i].GetValue() );
    }
  }



  void zTSharedList::Unarchive( zCArchiver& ar ) {
    zSTRING Instance;
    zSTRING Item;
    int ItemsCount;
    int Amount;

    ar.ReadString( "Instance", Instance );
    ar.ReadInt( "ItemsCount", ItemsCount );

    NpcInstance = A Instance;
    Npc = ogame->GetGameWorld()->SearchVobByName( Instance )->CastTo<oCNpc>();

    for( int i = 0; i < ItemsCount; i++ ) {
      ar.ReadString( "Item", Item );
      ar.ReadInt( "Amount", Amount );

      Items.Insert( Item, Amount );
    }
  }









  zTSharedList& zTShareManager::GetSharedList( oCNpc* npc ) {
    string instanceName = npc->GetInstanceName();
    uint index = SharedLists.SearchEqualSorted( instanceName );
    if( index != Invalid )
      return SharedLists[index];

    zTSharedList& sharedList = SharedLists.CreateSorted( instanceName );
    sharedList.NpcInstance = instanceName;
    sharedList.Npc = npc;

    return sharedList;
  }



  void zTShareManager::Insert( oCNpc* npc, oCItem* item, int amount ) {
    zTSharedList& sharedList = GetSharedList( npc );
    sharedList.Insert( item, amount );
  }



  void zTShareManager::Remove( oCNpc* npc, oCItem* item, int amount ) {
    zTSharedList& sharedList = GetSharedList( npc );
    sharedList.Remove( item, amount );

    if( sharedList.Items.IsEmpty() )
      SharedLists.Remove( sharedList );
  }



  int zTShareManager::CanTake( oCNpc* npc, oCItem* item ) {
    string npcInstanceName = npc->GetInstanceName();
    uint index = SharedLists.SearchEqualSorted( npcInstanceName );
    if( index == Invalid )
      return 0;

    zTSharedList& sharedList = SharedLists[index];
    string itemInstanceName = item->GetInstanceName();
    auto& pair = sharedList.Items[itemInstanceName];
    if( pair.IsNull() )
      return 0;

    return pair.GetValue();
  }


  static int GetPartyMemberIndex() {
    zCPar_Symbol* sym = parser->GetSymbol( "AIV_PARTYMEMBER" );
    if( !sym )
#if ENGINE >= Engine_G2
      return 15;
#else
      return 36;
#endif

    int id;
    sym->GetValue( id, 0 );
    return id;
  }

  bool zTShareManager::CanShareWith( oCNpc* npc ) {
    static int AIV_PARTYMEMBER = GetPartyMemberIndex();
    if( npc->aiscriptvars[AIV_PARTYMEMBER] )
      return true;

    string instanceName = npc->GetInstanceName();
    return SharedLists.HasEqualSorted( instanceName );
  }

  string GetSlotNameByID( uint ID ) {
    if( ID > 0 )
      return "savegame" + A ID;

    if( ID == 0 )
      return "quicksave";

    return "current";
  }

  string GetArchivePath() {
    int slotID = SaveLoadGameInfo.slotID;
    string savesDir = zoptions->GetDirString( zTOptionPaths::DIR_SAVEGAMES );
    string slotDir = GetSlotNameByID( SaveLoadGameInfo.slotID ); // slotID < 0 ? "Current" : "savegame" + A SaveLoadGameInfo.slotID;
    string archivePath = string::Combine( "%s\\%s\\Equipments.sav", savesDir, slotDir );
    return archivePath;
  }



  void zTShareManager::Save() {
    int slotID = SaveLoadGameInfo.slotID;
    if( slotID < 0 )
      return;

    zCArchiver* ar = zarcFactory->CreateArchiverWrite( Z GetArchivePath(), zARC_MODE_ASCII, 0, 0 );
    if( !ar )
      return;

    ar->WriteInt( "EquipmentsCount", SharedLists.GetNum() );
    for( uint i = 0; i < SharedLists.GetNum(); i++ )
      SharedLists[i].Archive( *ar );

    ar->Close();
    ar->Release();
  }



  void zTShareManager::Load() {
    int slotID = SaveLoadGameInfo.slotID;
    if( slotID < 0 ) {
      if( slotID == -2 )
        SharedLists.Clear();

      return;
    }

    SharedLists.Clear();

    zCArchiver* ar = zarcFactory->CreateArchiverRead( Z GetArchivePath(), 0 );
    if( !ar )
      return;

    int EquipmentsCount;
    ar->ReadInt( "EquipmentsCount", EquipmentsCount );

    for( int i = 0; i < EquipmentsCount; i++ ) {
      zTSharedList& equipmentList = SharedLists.Create();
      equipmentList.Unarchive( *ar );
    }

    SharedLists.QuickSort();
    ar->Close();
    ar->Release();
  }
}