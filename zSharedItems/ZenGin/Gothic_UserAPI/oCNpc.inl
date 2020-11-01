// Supported with union (c) 2020 Union team

// User API for oCNpc
// Add your methods here

void StartTalkingWith_Union( oCNpc* npc );
void StopTalkingWith_Union();
void SetTalkingWith_Union( oCNpc* npc, int talking );
void UnequipItem_Union( oCItem* );
int oCNpc::EV_ProcessInfos_Union( oCMsgConversation* csg );
void EquipMagic( oCItem* item );
void UnEquipItemByFlag( int flag );
int GetEquipedFlagItemsCount( int flag );