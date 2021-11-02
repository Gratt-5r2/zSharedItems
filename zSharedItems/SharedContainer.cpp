// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  const int HeroUID               = 0xFFFFFFFE;
  static bool s_SymbolOwnerByHero = false;
  static int s_Sharing            = False;


  static int Inventory_GetCategory( int mainFlag ) {
    switch( mainFlag ) {
      case ITM_CAT_NF:     return (INV_COMBAT);
      case ITM_CAT_FF:     return (INV_COMBAT);
      case ITM_CAT_MUN:    return (INV_COMBAT);
      case ITM_CAT_ARMOR:  return (INV_ARMOR);
      case ITM_CAT_FOOD:   return (INV_FOOD);
      case ITM_CAT_DOCS:   return (INV_DOCS);
      case ITM_CAT_POTION: return (INV_POTION);
      case ITM_CAT_RUNE:   return (INV_RUNE);
      case ITM_CAT_MAGIC:  return (INV_MAGIC);
      case ITM_CAT_NONE:   return (INV_OTHER);
    }

    return(INV_OTHER);
  }



  void oCSharedContainer::ItemIsNotYours() {
    ogame->GetTextView()->Printwin( Dia_IsNotYours + "\n" );
    player->GetModel()->StartAnimation( "T_DONTKNOW" );
  }



  void oCSharedContainer::SetOwner( oCNpc* owner ) {
    npc = owner;
#if ENGINE <= Engine_G1A
    SetContentID( npc->inventory2.invnr );
#endif
  }



  void oCSharedContainer::RemoveEquipedItems() {
    if( npc ) {
      npc->UnequipItem( npc->GetEquippedArmor() );
      npc->UnequipItem( npc->GetEquippedMeleeWeapon() );
      npc->UnequipItem( npc->GetEquippedRangedWeapon() );
    }
  }


  
  void oCSharedContainer::UpdateEquipedItems() {
    if( npc ) {
      npc->EquipBestArmor();
      npc->EquipBestWeapon( ITM_CAT_NF );
      npc->EquipBestWeapon( ITM_CAT_FF );
    }
  }



  void oCSharedContainer::OpenPassive( int x, int y, oTItemListMode mode ) {
    s_Sharing = True;

    if( npc ) {
      SetContents( npc->inventory2.contents );
      npc->inventory2.UnpackAllItems();
      npc->UnequipItem( npc->GetEquippedMeleeWeapon() );
      npc->UnequipItem( npc->GetEquippedRangedWeapon() );
#if ENGINE >= Engine_G2
      SetName( npc->name[0] );
#endif
    }

    oCItemContainer::OpenPassive( x, y, mode );
  }



  oCItem* oCSharedContainer::Insert( oCItem* item ) {
    int amount = item->amount;
    bool armor = item->IsArmor();
    bool magic = item->IsMagic();
    oCItem* newItem = oCItemContainer::Insert( item );

    ShareManager.Insert( npc, newItem, amount );

    if( armor )
      npc->EquipBestArmor();

    if( magic )
      npc->EquipMagic( newItem );
    
    return newItem;
  }



  inline int ExtractHeaderFlag( int flag ) {
    if( flag & ITM_FLAG_RING )   return ITM_FLAG_RING;
    if( flag & ITM_FLAG_AMULET ) return ITM_FLAG_AMULET;
#if ENGINE == Engine_G2A
    if( flag & ITM_FLAG_BELT )   return ITM_FLAG_BELT;
#endif
    return 0;
  }



  int oCNpc::GetEquipedFlagItemsCount( int flag ) {
    int count = 0;
#if ENGINE >= Engine_G2
    auto list = inventory2.inventory.next;
#else
    auto list = inventory2.inventory[inventory2.invnr].next;
#endif
    while( list ) {
      oCItem* item = list->data;
      if( item->flags & flag && item->IsEquiped() )
        count++;

      list = list->next;
    }

    return count;
  }



  void oCNpc::UnEquipItemByFlag( int flag ) {
    int equipedMax = flag == ITM_FLAG_RING ? 2 : 1;
    int equipCount = GetEquipedFlagItemsCount( flag );
    if( equipCount < equipedMax )
      return;

#if ENGINE >= Engine_G2
    auto list = inventory2.inventory.next;
#else
    auto list = inventory2.inventory[inventory2.invnr].next;
#endif
    while( list ) {
      oCItem* item = list->data;
      if( item->HasFlag( flag ) && item->IsEquiped() )
        UnequipItem( item );

      list = list->next;
    }
  }



  void oCNpc::UnEquipItemByMainFlag( int cat ) {
#if ENGINE >= Engine_G2
    auto list = inventory2.inventory.next;
    while( list ) {
      oCItem* item = list->data;
      if( item->mainflag == cat && item->IsEquiped() )
        UnequipItem( item );

      list = list->next;
    }
#else
    int category = Inventory_GetCategory( cat );
    auto list = inventory2.inventory[category].next;
    while( list ) {
      oCItem* item = list->data;
      UnequipItem( item );
      list = list->next;
    }
#endif
  }



  void oCNpc::EquipMagic( oCItem* item ) {
    UnEquipItemByFlag( ExtractHeaderFlag( item->flags ) );
    Equip( item );
  }



  void oCSharedContainer::Remove( oCItem* item ) {
    if( item->IsEquiped() && item->IsWeapon() )
      return;

    if( !ShareManager.CanTake( npc, item ) )
      return ItemIsNotYours();
    
    bool equiped = item->IsEquiped();
    bool armor   = item->IsArmor();
    if( equiped )
      npc->UnequipItem( item );

    oCItemContainer::Remove( item );
    if( equiped && armor )
      npc->EquipBestArmor();
  }



  oCItem* oCSharedContainer::Remove( oCItem* item, int amount ) {
    if( item->IsEquiped() && item->IsWeapon() )
      return Null;

    int amountMax = ShareManager.CanTake( npc, item );

    if( !amountMax ) {
      ItemIsNotYours();
      return Null;
    }

    if( amount > amountMax )
      amount = amountMax;

    oCItem* removed = oCItemContainer::Remove( item, amount );
    ShareManager.Remove( npc, item, amount );
    return removed;
  }



  oCItem* oCSharedContainer::RemoveByPtr( oCItem* item, int amount ) {
    if( !ShareManager.CanTake( npc, item ) ) {
      ItemIsNotYours();
      return Null;
    }

    return oCItemContainer::RemoveByPtr( item, amount );
  }



  void oCSharedContainer::Close() {
    UpdateEquipedItems();
    oCItemContainer::Close();
    s_Sharing = False;
  }



  void OpenShareContainer( oCNpc* trader ) {
    static oCSharedContainer* container = new oCSharedContainer();
    container->SetOwner( trader );

#if ENGINE >= Engine_G2
    container->SetName( trader->GetName( 0 ) );
    container->Open( 0, 0, INV_MODE_CONTAINER );
    player->OpenInventory( INV_MODE_DEFAULT );
#else
    container->Open( 0, 0, oCItemContainer::oTItemListMode::FULLSCREEN );
    player->OpenInventory();
    container->SetContentID( player->inventory2.invnr );
#endif
  }



#if ENGINE <= Engine_G1A
  void oCSharedContainer::SetContentID( int index ) {
    if( invnr == index )
      return;

    invnr = index != -1 ? index : 1;
    SetContents( &npc->inventory2.inventory[invnr] );
    npc->inventory2.SwitchToCategory( invnr );
  }



  void oCSharedContainer::SetCategoryOnRightContainer() {
    auto next = dynamic_cast<oCNpcInventory*>(GetNextContainerRight( this ));
    if( next ) {
      next->SwitchToCategory( invnr );
      npc->inventory2.invnr = invnr;
    }
  }



  void oCSharedContainer::DrawCategory() {
    if( !IsActive() ) {
      auto next = dynamic_cast<oCNpcInventory*>( GetNextContainerRight( this ) );
      if( next )
        SetContentID( next->invnr );
    }

    oCItemContainer::DrawCategory();
  }



  int oCSharedContainer::HandleEvent( int key ) {
    if( zinput->IsBinded( GAME_LEFT, key ) || zinput->IsBinded( GAME_STRAFELEFT, key ) ) {
      if( invnr > 1 ) {
        SetContentID( invnr - 1 );
        return True;
      }
    }

    else if( zinput->IsBinded( GAME_RIGHT, key ) || zinput->IsBinded( GAME_STRAFERIGHT, key ) ) {
      if( invnr < INV_MAX - 1 ) {
        SetContentID( invnr + 1 );
        return True;
      }
    }

    else if( key == MOUSE_BUTTONRIGHT || zinput->IsBinded( GAME_END, key ) || zinput->IsBinded( GAME_INVENTORY, key ) ) {
      this->Close();
    }

    return oCItemContainer::HandleEvent( key );
  }




  FASTHOOK( oCNpcInventory, HandleEvent );
  int oCNpcInventory::HandleEvent_Union( int key ) {
    if( zinput->IsBinded( GAME_SMOVE, key ) ) {
      if( dynamic_cast<oCSharedContainer*>(GetNextContainerLeft( this )) ) {
        TransferItem( -1, zKeyPressed( KEY_LSHIFT ) ? 100 : 10 );
        return True;
      }
    }

    return THISCALL( Hook_oCNpcInventory_HandleEvent )( key );
  }
#endif
}