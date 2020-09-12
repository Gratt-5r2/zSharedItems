// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
#if ENGINE >= Engine_G2
	typedef int oTItemListMode;
#endif

	class oCSharedContainer : public oCItemContainer {
	public:

		void ItemIsNotYours();
		void SetOwner( oCNpc* owner );

		void RemoveEquipedItems();
		void UpdateEquipedItems();

		oCItem* InsertByOwner( oCItem* item );
		virtual void OpenPassive( int x, int y, oTItemListMode mode );
		virtual oCItem* Insert( oCItem* item );
		virtual void Remove( oCItem* item );
		virtual oCItem* Remove( oCItem* item, int amount );
		virtual oCItem* RemoveByPtr( oCItem* item, int amount );
		virtual void Close();
	};

	string Dia_IsNotYours;
	string Dia_ShareItems;
}