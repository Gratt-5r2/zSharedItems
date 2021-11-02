// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	struct zTSharedList {
		string NpcInstance;
		oCNpc* Npc;
		bool Temporary;

		Map<string, int> Items;

		oCNpc* GetSynonymNpc( const string& instanceName );
		void Insert( const string& instanceName, int amount );
		void Insert( oCItem* item, int amount );
		void Remove( oCItem* item, int amount );
		void UnequipAll();
		void EquipAll();
		void CreateInvertedCopy( const string& instanceName );
		bool IsNpcValid();
		bool operator == ( const string& other ) const;
		bool operator >  ( const string& other ) const;
		bool operator <  ( const string& other ) const;
		operator const string& () const;

		void TransferNpcInstance();
		void Archive( zCArchiver& ar );
		bool Unarchive( zCArchiver& ar );
	};



	struct zTShareManager {
		bool Insorted;
		Array<zTSharedList> SharedLists;

		zTSharedList& GetSharedList( const string& npcInstName );
		zTSharedList& GetSharedList( oCNpc* npc );
		void Insert( oCNpc* npc, oCItem* item, int amount );
		void Insert( oCNpc* npc, const zSTRING& itemInstName, int amount );
		void Remove( oCNpc* npc, oCItem* item, int amount );
		int CanTake( oCNpc* npc, oCItem* item );
		bool CanShareWith( oCNpc* npc );


		void Save();
		void Load();
	};

	zTShareManager ShareManager;
}