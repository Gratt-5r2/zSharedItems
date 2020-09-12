// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	struct zTSharedList {
		string NpcInstance;
		oCNpc* Npc;

		Map<string, int> Items;

		void Insert( oCItem* item, int amount );
		void Remove( oCItem* item, int amount );
		bool operator == ( const string& other ) const;
		bool operator >  ( const string& other ) const;
		bool operator <  ( const string& other ) const;
		operator const string& () const;

		void Archive( zCArchiver& ar );
		void Unarchive( zCArchiver& ar );
	};



	struct zTShareManager {
		Array<zTSharedList> SharedLists;

		zTSharedList& GetSharedList( oCNpc* npc );
		void Insert( oCNpc* npc, oCItem* item, int amount );
		void Remove( oCNpc* npc, oCItem* item, int amount );
		int CanTake( oCNpc* npc, oCItem* item );
		bool CanShareWith( oCNpc* npc );


		void Save();
		void Load();
	};

	zTShareManager ShareManager;
}