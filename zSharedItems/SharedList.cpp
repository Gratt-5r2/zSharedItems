// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  static Array<string> InstanceSynonyms;

  Array<string> GetSynonymsList( const string& instanceName ) {
    static Array<string> empty;
    for( uint i = 0; i < InstanceSynonyms.GetNum(); i++ ) {
      string& synList = InstanceSynonyms[i];
      Array<string> synArray = synList.Split( "|" );
      for( uint j = 0; j < synArray.GetNum(); j++ ) {
        if( instanceName == synArray[j] ) {
          synArray.RemoveAt( j );
          return synArray;
        }
      }
    }

    return empty;
  }



  void zTSharedList::Insert( const string& instanceName, int amount ) {
    auto& pair = Items[instanceName];

    if( pair.IsNull() )
      return Items.Insert( instanceName, amount );

    pair.GetValue() += amount;
  }



  void zTSharedList::Insert( oCItem* item, int amount ) {
    string instanceName = item->GetInstanceName();
    Insert( instanceName, amount );
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



  static oCNpc* SearchNpcInWorld( const string& instanceName ) {
    return ogame->GetGameWorld()->SearchVobByName( instanceName )->CastTo<oCNpc>();
  }



  static oCItem* CreateItemWorld( const string& instanceName ) {
    int index = parser->GetIndex( instanceName );
    return (oCItem*)ogame->GetGameWorld()->CreateVob( zVOB_TYPE_ITEM, index );
  }



  oCNpc* zTSharedList::GetSynonymNpc( const string& instanceName ) {
    Array<string> synonyms = GetSynonymsList( instanceName );
    for( uint i = 0; i < synonyms.GetNum(); i++ ) {
      string& synonym = synonyms[i];
      oCNpc* npc = SearchNpcInWorld( synonym );
      if( npc )
        return npc;
    }

    return Null;
  }



  bool zTSharedList::Unarchive( zCArchiver& ar ) {
    zSTRING Instance;
    zSTRING Item;
    int ItemsCount;
    int Amount;
    Temporary = false;

    ar.ReadString( "Instance", Instance );
    ar.ReadInt( "ItemsCount", ItemsCount );

    NpcInstance = A Instance;
    Npc = SearchNpcInWorld( Instance );

    for( int i = 0; i < ItemsCount; i++ ) {
      ar.ReadString( "Item", Item );
      ar.ReadInt( "Amount", Amount );
      Items.Insert( Item, Amount );
      if( Amount < 0 )
        Temporary = true;
    }

    if( Temporary && Npc ) {
      auto& itemsArray = Items.GetArray();
      UnequipAll();
      for( uint i = 0; i < itemsArray.GetNum(); i++ ) {
        const string& instance = itemsArray[i].GetKey();
        const int& amount      = itemsArray[i].GetValue();
        Npc->RemoveFromInv( instance, -amount );
      }

      EquipAll();
      return false;
    }

    return true;
  }



  bool zTSharedList::IsNpcValid() {
    return Npc && !Npc->IsDead();
  }



  void zTSharedList::UnequipAll() {
    Npc->UnEquipItemByMainFlag( ITM_CAT_NF );
    Npc->UnEquipItemByMainFlag( ITM_CAT_FF );
    Npc->UnEquipItemByMainFlag( ITM_CAT_ARMOR );
  }



  void zTSharedList::EquipAll() {
    Npc->EquipBestArmor();
    Npc->EquipBestWeapon( ITM_CAT_NF );
    Npc->EquipBestWeapon( ITM_CAT_FF );
    Npc->InitModel();
  }



  void zTSharedList::TransferNpcInstance() {
    if( Temporary )
      return;

    Array<string> synonyms = GetSynonymsList( NpcInstance );
    for( uint i = 0; i < synonyms.GetNum(); i++ ) {
      string synonym = synonyms[i];
      Npc = SearchNpcInWorld( synonym );
      if( Npc ) {
        cmd << "Transfer Npc created: " << NpcInstance << " -> " << synonym << endl;
        string oldName = NpcInstance;
        NpcInstance = synonym;
        CreateInvertedCopy( oldName );

        auto& itemsArray = Items.GetArray();
        for( uint j = 0; j < itemsArray.GetNum(); j++ ) {
          const string& instance = itemsArray[j].GetKey();
          const int& amount = itemsArray[j].GetValue();

          int itemIndex = parser->GetIndex( instance );
          int itemAmout = Npc->inventory2.GetAmount( itemIndex );
          if( itemAmout < amount )
            Npc->CreateItems( itemIndex, amount - itemAmout );
        }

        EquipAll();
        break;
      }
    }
  }



  void zTSharedList::CreateInvertedCopy( const string& instanceName ) {
    zTSharedList& copied = ShareManager.GetSharedList( instanceName );
    auto& itemsArray = Items.GetArray();
    for( uint i = 0; i < itemsArray.GetNum(); i++ ) {
      const string& instance = itemsArray[i].GetKey();
      const int& amount      = itemsArray[i].GetValue();
      copied.Insert( instance, -amount );
    }

    cmd << "Inverted copy created: " << instanceName << endl;
  }



  zTSharedList& zTShareManager::GetSharedList( const string& npcInstName ) {
    uint index = Insorted ? SharedLists.SearchEqual( npcInstName ) : SharedLists.SearchEqualSorted( npcInstName );
    if( index != Invalid )
      return SharedLists[index];

    zTSharedList& sharedList = Insorted ? SharedLists.Create() : SharedLists.CreateSorted( npcInstName );
    sharedList.NpcInstance = npcInstName;
    sharedList.Npc = Null;

    return sharedList;
  }



  zTSharedList& zTShareManager::GetSharedList( oCNpc* npc ) {
    string instanceName = npc->GetInstanceName();
    uint index = Insorted ? SharedLists.SearchEqual( instanceName ) : SharedLists.SearchEqualSorted( instanceName );
    if( index != Invalid )
      return SharedLists[index];

    zTSharedList& sharedList = Insorted ? SharedLists.Create() : SharedLists.CreateSorted( instanceName );
    sharedList.NpcInstance = instanceName;
    sharedList.Npc = npc;

    return sharedList;
  }



  void zTShareManager::Insert( oCNpc* npc, oCItem* item, int amount ) {
    zTSharedList& sharedList = GetSharedList( npc );
    sharedList.Insert( item, amount );
  }



  void zTShareManager::Insert( oCNpc* npc, const zSTRING& itemInstName, int amount ) {
    zTSharedList& sharedList = GetSharedList( npc );
    sharedList.Insert( itemInstName, amount );
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

  string GetSlotNameByID( int ID ) {
    if( ID > 0 )
      return "savegame" + A ID;

    if( ID == 0 )
      return "quicksave";

    return "current";
  }

  string GetArchivePath() {
    int slotID = SaveLoadGameInfo.slotID;
    string savesDir = zoptions->GetDirString( zTOptionPaths::DIR_SAVEGAMES );
    string slotDir = GetSlotNameByID( slotID ); // slotID < 0 ? "Current" : "savegame" + A SaveLoadGameInfo.slotID;
    string archivePath = string::Combine( "%s\\%s\\Equipments.sav", savesDir, slotDir );
    return archivePath;
  }



  void zTShareManager::Save() {
    for( uint i = 0; i < SharedLists.GetNum(); i++ ) {
      auto& sharedList = SharedLists[i];
      oCNpc* npc = sharedList.Npc;
      if( npc && npc->attribute[NPC_ATR_HITPOINTS] <= 0 )
        SharedLists.RemoveAt( i-- );
    }

    int slotID = SaveLoadGameInfo.slotID;
    if( slotID == -2 )
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
      if( slotID == -2 ) {
        SharedLists.Clear();
        return;
      }
    }

    SharedLists.Clear();
    Insorted = true;

    zCArchiver* ar = zarcFactory->CreateArchiverRead( Z GetArchivePath(), 0 );
    if( !ar )
      return;

    int EquipmentsCount;
    ar->ReadInt( "EquipmentsCount", EquipmentsCount );

    for( int i = 0; i < EquipmentsCount; i++ ) {
      zTSharedList equipmentList;
      if( equipmentList.Unarchive( *ar ) )
        SharedLists.Insert( equipmentList );
    }

    uint count = SharedLists.GetNum();
    for( uint i = 0; i < count; i++ ) {
      zTSharedList& equipmentList = SharedLists[i];
      if( !equipmentList.IsNpcValid() )
        equipmentList.TransferNpcInstance();
    }

    Insorted = false;
    SharedLists.QuickSort();
    ar->Close();
    ar->Release();
  }
}