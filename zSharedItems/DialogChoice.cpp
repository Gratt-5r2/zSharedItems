// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	static int s_ShareID				= 0xFFFFFFFE;
	static oCInfo* s_SharedInfo = Null;
	static oCInfo* s_LastInfo		= Null;
	static bool		 s_ShowChoice = false;


	HOOK Hook_oCInfoManager_GetInfoUnimportant AS( &oCInfoManager::GetInfoUnimportant, &oCInfoManager::GetInfoUnimportant_Union );

	oCInfo* oCInfoManager::GetInfoUnimportant_Union( oCNpc* asker, oCNpc* asked, int index ) {
		int additional = s_ShowChoice ? 1 : 0;
		if( additional && index == 0 )
			return s_SharedInfo;

		return THISCALL( Hook_oCInfoManager_GetInfoUnimportant )(asker, asked, index - additional);
	}



	HOOK Hook_oCInfoManager_GetInfoCountUnimportant AS( &oCInfoManager::GetInfoCountUnimportant, &oCInfoManager::GetInfoCountUnimportant_Union );

	int oCInfoManager::GetInfoCountUnimportant_Union( oCNpc* asker, oCNpc* asked ) {
		s_ShowChoice = ShareManager.CanShareWith( asked );

		int additional = s_ShowChoice ? 1 : 0;
		return THISCALL( Hook_oCInfoManager_GetInfoCountUnimportant )(asker, asked) + additional;
	}


	HOOK Hook_oCInformationManager_Update AS( &oCInformationManager::Update, &oCInformationManager::Update_Union );

	extern void OpenShareContainer( oCNpc* trader );

	void oCInformationManager::Update_Union() {
		if( s_Sharing )
			return;

		THISCALL( Hook_oCInformationManager_Update )();
	}



	HOOK Hook_oCInformationManager_OnTradeBegin AS( &oCInformationManager::OnTradeBegin, &oCInformationManager::OnShareBegin );

	void oCInformationManager::OnShareBegin() {
		if( s_LastInfo && s_LastInfo->pd.trade == s_ShareID ) {
			OpenShareContainer( Npc );
			return;
		}

		THISCALL( Hook_oCInformationManager_OnTradeBegin )();
	}



	HOOK Hook_oCInformationManager_OnInfo AS( &oCInformationManager::OnInfo, &oCInformationManager::OnInfo_Union );

	void oCInformationManager::OnInfo_Union( oCInfo* info ) {
		s_LastInfo = info;
		THISCALL( Hook_oCInformationManager_OnInfo )(info);
	}




	void CreateSharedInformation() {
		s_SharedInfo								 = new oCInfo();
		s_SharedInfo->pd.description = Z Dia_ShareItems;
		s_SharedInfo->pd.trade			 = s_ShareID;
	}
}