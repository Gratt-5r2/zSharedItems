// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  const int HeroUID               = 0xFFFFFFFE;
  static bool s_SymbolOwnerByHero = false;
  static int s_Sharing            = False;



  void oCSharedContainer::ItemIsNotYours() {
    ogame->GetTextView()->Printwin( Dia_IsNotYours + "\n" );
  }



  void oCSharedContainer::SetOwner( oCNpc* owner ) {
    npc = owner;
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
    oCItem* newItem = oCItemContainer::Insert( item );
    ShareManager.Insert( npc, newItem, amount );

    if( armor )
      npc->EquipBestArmor();

    return newItem;
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
#endif
  }
}