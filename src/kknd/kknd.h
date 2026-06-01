
#include <windows.h>
#include <math.h>

#include <ddraw.h>
#include <dplay.h>
#include <dsound.h>


struct KKND::TaskLocal;
struct KKND::Task;
struct KKND::BoxdAabb;
struct KKND::BoxdCollisionShape;
struct KKND::CplcSpawnParams;
struct KKND::CplcEntity;
struct KKND::CplcEntityInViewport;
struct KKND::MobdImageData;
struct KKND::MobdSprtImage;
struct KKND::MobdAnimFrame;
struct KKND::Entity;
struct KKND::MobdAnimation;
struct KKND::BoxdCollisionHandler;
struct KKND::RenderViewport;
struct KKND::OilPatch;
struct KKND::Unit;
struct KKND::RenderNode;
struct KKND::UnitStats;
struct KKND::Turret;
struct KKND::UnitEscortNode;
struct KKND::UnitAttachment;
struct KKND::UnitProjectileType;
union KKND::UnitUnion1;
struct KKND::CursorUnitSelection;
struct KKND::BuildingPlannerPayload;
struct KKND::SidebarFactoryProductionOption;
struct KKND::Sidebar;
struct KKND::ProductionSharedState;
struct KKND::Glyph;
struct KKND::AiUnitNode;
struct KKND::AiWandererNode;
struct KKND::AiAttackerNode;
struct KKND::AiBuildNode;
struct KKND::AiDrillrigNode;
struct KKND::AiTankerNode;
struct KKND::AiPowerPlantNode;
struct KKND::AiEnemyNode;
struct KKND::AiSquadNode;
struct KKND::AiBuildOrderNode;
struct KKND::AiBuildingPlacementNode;
struct KKND::RenderImage;
struct KKND::MenuWidget;
struct KKND::GlyphDesc;
struct KKND::FontMobd;



/* 161 */
enum KKND::TaskChannel : unsigned __int32
{
  TaskChannel_None = 0x0,
  TaskChannel_MainMenu = 0x1,
  TaskChannel_MultiplayerProvider = 0x2,
  TaskChannel_4_Menu = 0x4,
  TaskChannel_5_Multiplayer = 0x5,
  TaskChannel_6_Multiplayer = 0x6,
  TaskChannel_7_Menu = 0x7,
  TaskChannel_KaosMenu = 0x8,
  TaskChannel_KaosLobbyWatch = 0x9,
  TaskChannel_10_Menu = 0xA,
  TaskChannel_PlayMission = 0xD,
  TaskChannel_14_Menu = 0xE,
  TaskChannel_CampaignFactionSelect = 0xF,
  TaskChannel_16_Menu = 0x10,
  TaskChannel_MenuCancel = 0x11,
  TaskChannel_TextInput = 0x12,
  TaskChannel_MainMenuLoad = 0x13,
  TaskChannel_UnitEvents = 0x9876,
  TaskChannel_SidebarButton = 0xBABA,
  TaskChannel_SidebarHelp = 0xBBBB,
  TaskChannel_SidebarPlaceholder = 0xCACA,
  TaskChannel_Units = 0xEAEA,
  TaskChannel_Outpost = 0xCA000000,
  TaskChannel_Clanhall = 0xCA000001,
  TaskChannel_MachineShop = 0xCA000002,
  TaskChannel_Blacksmith = 0xCA000004,
  TaskChannel_BeastEnclosure = 0xCA000005,
  TaskChannel_RepairBay = 0xCA000006,
  TaskChannel_ResearchLab = 0xCA00000A,
  TaskChannel_PowerPlant = 0xCA00000D,
  TaskChannel_Drillrig = 0xCA00000E,
  TaskChannel_Tanker = 0xCA00000F,
  TaskChannel_MobileDerrick = 0xCA000010,
  TaskChannel_unused_tanker_broadcast = 0xCA000012,
  TaskChannel_UpgradeProgress = 0xCA000013,
  TaskChannel_unused_CA000014 = 0xCA000014,
  TaskChannel_IngameMenuController = 0xDA000000,
  TaskChannel_UiSlider_or_SaveLoadIngame = 0xDA000001,
  TaskChannel_IngameMenuButton = 0xDA000002,
  TaskChannel_IngameMenuYesNo = 0xDA000003,
  TaskChannel_SaveLoad_Ingame = 0xDA000006,
  TaskChannel_SaveLoad_MainMenu = 0xDA000007,
  TaskChannel_SaveLoad = 0xDA000008,
  TaskChannel_Everyone = 0xFFFFFFFF,
};

/* 165 */
enum KKND::TaskMessageType : unsigned __int32
{
  TaskMessage_Attacked = 1497u,
  TaskMessage_MissionFailed = 1498u,
  TaskMessage_Crushed = 1499u,
  TaskMessage_DestroyAttachment = 1500u,
  TaskMessage_ReceiveDamage = 1503u,
  TaskMessage_GainExperience = 1505u,
  TaskMessage_Sabotage = 1506u,
  TaskMessage_EscortBegin_or_SaveGame = 1507u,
  TaskMessage_EscortRetaliate = 1508u,
  TaskMessage_EscortEnd_or_LoadGame = 1509u,
  TaskMessage_RepairTick = 1510u,
  TaskMessage_UnitSelected_or_UiLeftClick = 1511u,
  TaskMessage_UnitDeselected_or_SaveLoadScrollDown_or_ShowNotificationBox = 1512u,
  TaskMessage_SiderbarRightClick = 1513u,
  TaskMessage_SidebarForceClose = 1514u, ///< e.g a prod building is destroyed
  TaskMessage_ShowHint_or_SaveLoadScrollUp = 1515u,
  TaskMessage_HideHint = 1516u,
  TaskMessage_Destroy = 1517u,
  TaskMessage_1518 = 1518u,
  TaskMessage_AirstrikeSetTargetingMode = 1519u,
  TaskMessage_AirstrikeClearTargetingMode = 1520u,
  TaskMessage_UnitCreated = 1521u,
  TaskMessage_BuildingPlacementModeBegin_or_BackToMenu = 1522u,
  TaskMessage_AttackOrder_or_QuitGame = 1523u,
  TaskMessage_MoveOrder_or_SoundSettings_or_DoSaveGame = 1524u,
  TaskMessage_MissionAccomplished = 1524u, ///< Duplicated enum value as far as I understand, used depending on context
  TaskMessage_GuardAreaOrder = 1525u,
  TaskMessage_Infiltrate_or_ShowBriefing = 1526u,
  TaskMessage_OpenBriefing = 1526u,
  TaskMessage_Follow_or_RestartGame = 1527u,
  TaskMessage_RestartLevel = 1527u,
  TaskMessage_Retreat_or_CancelUi = 1528u,
  TaskMessage_CloseDialog = 1528u,
  TaskMessage_AdvanceConstructionStage = 1529u,
  TaskMessage_ToggleIngameMenu = 1530u,
  TaskMessage_MissionOutcomePopup = 1531u,
  TaskMessage_PauseGame = 1532u,
  TaskMessage_ResumeGame = 1533u,
  TaskMessage_UnitReady = 1535u,
  TaskMessage_BuildingComplete = 1537u,
  TaskMessage_BroadcastBuildingComplete = 1538u,
  TaskMessage_PowerPlantDown = 1539u,
  TaskMessage_DrillrigDown = 1540u,
  TaskMessage_TankerAssignedNewPowerPlant = 1541u,
  TaskMessage_TankerAssignedNewDrillrig = 1542u,
  TaskMessage_UpgradeComplete = 1543u,
  TaskMessage_UpgradeStarted = 1544u,
  TaskMessage_UpgradeCancelled = 1545u,
  TaskMessage_RepairBayAssigned = 1546u,
  TaskMessage_TechnicianAssigned = 1547u,
  TaskMessage_SidebarRefreshOptions = 1548u,
  TaskMessage_SpawnTanker_or_DoLoadGame_or_LobbyRefreshSettings = 1549u,
  TaskMessage_LoadGame = 1549u,
  TaskMessage_TowerReady = 1551u,
  TaskMessage_SoundPanTransitionComplete = 4294967291u,
  TaskMessage_SoundVolumeTransitionComplete = 4294967292u,
  TaskMessage_SoundCleanupComplete = 4294967293u,
  TaskMessage_MouseHover = 4294967294u,
  TaskMessage_AnimationAdvanced = 4294967295u,
  TaskMessage_1534 = 1534u,
};

/* 162 */
struct KKND::TaskMessage
{
  KKND::TaskMessage *next;
  struct KKND::Task *task;
  KKND::TaskMessageType type;
  void *payload;
};

/* 163 */
enum KKND::TaskKind : unsigned __int32
{
  TaskKind_Coroutine = 0,
  TaskKind_Callback = 1,
};

/* 203 */
enum KKND::TaskNetzFlags : unsigned __int32
{
  Task_CanRunDuringSync = 0x1,
};

/* 374 */
enum KKND::TaskEvents : unsigned __int32
{
  TaskEvent_OilDepleted = 0x1,
  TaskEvent_AnimCompleted = 0x10000000,
  TaskEvent_AnimAdvanced = 0x40000,
  TaskEvent_ProjectileHit = 0x2,
  TaskEvent_CplcSpawn = 0x20000,
  TaskEvent_CollisionPosZ = 0x80000,
  TaskEvent_CollisionNegZ = 0x100000,
  TaskEvent_CollisionPosY = 0x200000,
  TaskEvent_CollisionNegY = 0x400000,
  TaskEvent_CollisionPosX = 0x800000,
  TaskEvent_CollisionNegX = 0x1000000,
  TaskEvent_SpeedLimitZ = 0x2000000,
  TaskEvent_SpeedLimitY = 0x4000000,
  TaskEvent_SpeedLimitX = 0x8000000,
  TaskEvent_Terminated = 0x20000000,
  TaskEvent_Message = 0x40000000,
  TaskEvent_Wake = 0x80000000,          ///< Sleep interval expired
};

/* 226 */
enum KKND::TaskWaitFlags : unsigned __int32
{
  TaskWait_OilDepleted = 0x1,
  TaskWait_Terminated = 0x20000000,
  TaskWait_Message = 0x40000000,
  TaskWait_Interval = 0x80000000,       ///< Sleep: Wake up after sleep interval
  TaskWait_Any = 0xC0000000,            ///< Yield: wake up on any condition
  TaskWait_AnimCompletion = 0x10000000, ///< Wake up when the current animation is completed
  TaskWait_AnimAdvancement = 0x40000,
  TaskWait_ProjectileHit = 0x2,
};

/* 235 */
typedef void (__fastcall *KKND::MessageHandler)(KKND::Task *receiver, KKND::Task *sender, KKND::TaskMessageType message, void *payload);

/* 178 */
typedef void (__cdecl *KKND::TaskFn)(KKND::Task *task);

/* 166 */
struct KKND::Task
{
  KKND::Task *next;
  KKND::Task *prev;
  KKND::TaskLocal *locals;
  KKND::TaskChannel channel;
  int _task_field_10;
  int sleep;
  KKND::TaskKind kind;
  KKND::TaskNetzFlags netz_flags;
  KKND::TaskEvents transient_events;    ///< events happened this task run - zeroed out on TSK_yield()
  KKND::TaskEvents sticky_events;       ///< sticky events that happenned over the entire lifetime (they are cleared manually in some circumstances)
  KKND::TaskWaitFlags wait_flags;
  int wait_filter;                      ///< wake filter is actually quite weird.. it serves both as a number of ticks to sleep but also it can have a bitmask of events that should NOT wake the yield
  KKND::TaskMessage *message_queue;
  KKND::MessageHandler message_handler;
  KKND::Entity *entity;
  void *ctx;
  KKND::TaskFn entry_point;
};

/* 164 */
struct __unaligned KKND::TaskLocal
{
  KKND::TaskLocal *next;
  KKND::TaskLocal *prev;
  char data[1];
};

/* 175 */
enum KKND::MobdId : unsigned __int32
{
  MobdId_Mute_AlchemyHall = 0,
  MobdId_Surv_Atv = 1,
  MobdId_Surv_BarrageCraft = 2,
  MobdId_Mute_BeastEnclosure = 3,
  MobdId_Mute_GiantBeetle = 4,
  MobdId_Mute_Berserker = 5,
  MobdId_some_letters_and_symbols = 6,
  MobdId_Surv_DirkBike = 7,
  MobdId_Mute_Blacksmith = 8,
  MobdId_TechBunker = 9,
  MobdId_Sidebar = 10,
  MobdId_Surv_AutocannonTank = 11,
  MobdId_Surv_CannonTower = 12,
  MobdId_Mute_Clanhall = 13,
  MobdId_Mute_ClanhallWagon = 14,
  MobdId_MissionOutcomeModal = 15,
  MobdId_Mute_MissileCrab = 16,
  MobdId_Cursors = 17,
  MobdId_Surv_DetentionCenter = 18,
  MobdId_Mute_DirewWolf = 19,
  MobdId_20 = 20,
  MobdId_Surv_General = 21,
  MobdId_Explosions = 22,
  MobdId_23_unused = 23,
  MobdId_Surv_AtvFlamethrower = 24,
  MobdId_Surv_Flamer = 25,
  MobdId_Font_Briefing = 26,
  MobdId_Font_Main = 27,
  MobdId_Gort = 28,
  MobdId_Mute_GrapeshotTower = 29,
  MobdId_IngameMenuUi = 30,
  MobdId_Mute_CrazyHarry = 31,
  MobdId_Mute_HoldingPen = 32,
  MobdId_Hut = 33,
  MobdId_Surv_Rifleman = 34,
  MobdId_Mute_Chieftan = 35,
  MobdId_PlasmaTank = 36,
  MobdId_Surv_MachineShop = 37,
  MobdId_Mute_WarMastadont = 38,
  MobdId_Mute_MobileDerrick = 39,
  MobdId_Mech = 40,
  MobdId_Mute_Mekanik = 41,
  MobdId_Mute_Menagerie = 42,
  MobdId_Mute_MachinegunNest = 43,
  MobdId_Surv_MissileBattery = 44,
  MobdId_Cursor = 45,
  MobdId_LevelSelectionButtons = 46,
  MobdId_Mute_MonsterTruck = 47,
  MobdId_Mute_OilTanker = 48,
  MobdId_Mute_PowerStation = 49,
  MobdId_Mute_Drillrig = 50,
  MobdId_OilPatch = 51,
  MobdId_Surv_Outpost = 52,
  MobdId_Surv_MobileOutpost = 53,
  MobdId_Surv_4x4Pickup = 54,
  MobdId_Mute_Pyromaniac = 55,
  MobdId_Surv_RepairBay = 56,
  MobdId_Surv_ResearchLab = 57,
  MobdId_Mute_Rioter = 58,
  MobdId_Surv_RpgLauncher = 59,
  MobdId_Mute_Bazooka = 60,
  MobdId_Mute_RotaryCannon = 61,
  MobdId_Surv_Saboteur = 62,
  MobdId_Surv_Sapper = 63,
  MobdId_Mute_GiantScorpion = 64,
  MobdId_Surv_MobileDerrick = 65,
  MobdId_SentinelDroid = 66,
  MobdId_Surv_GuardTower = 67,
  MobdId_Mute_Shotgunner = 68,
  MobdId_Schrap = 69,
  MobdId_Mute_BikeAndSidecar = 70,
  MobdId_Surv_Sniper = 71,
  MobdId_AcidSplash = 72,
  MobdId_Surv_OilTanker = 73,
  MobdId_Surv_PowerStation = 74,
  MobdId_Surv_Drillrig = 75,
  MobdId_Surv_Swat = 76,
  MobdId_Surv_AnacondaTank = 77,
  MobdId_Surv_Technician = 78,
  MobdId_MainMenuButtons = 79,
  MobdId_Font_Menu = 80,
  MobdId_Mute_Vandal = 81,
  MobdId_Mute_Wasp = 82,
  MobdId_Surv_Bomber = 83,
  MobdId_Invalid = -1,
};

/* 380 */
struct KKND::EntityBuildingContext
{
  int hitpoints;                        ///< HP inhereted from the deploying unit (base, drillrig)
  int _unused;
};

/* 381 */
struct KKND::EntityProjectileContext
{
  KKND::Unit *attacker;                 ///< who fired the projectile
  int attacker_unit_id;
};

/* 382 */
union KKND::EntityContext
{
  KKND::EntityBuildingContext building_ctx;
  KKND::EntityProjectileContext projectile_ctx;
  KKND::MenuWidget *widget;
};

/* 198 */
struct KKND::Entity
{
  KKND::Entity *next;
  KKND::Entity *prev;
  KKND::Entity *parent;
  KKND::MobdId mobd_id;
  int x;
  int y;
  int z;
  int x_speed;
  int y_speed;
  int z_speed;
  int x_acceleration;
  int y_acceleration;
  int z_acceleration;
  int x_speed_limit;
  int y_speed_limit;
  int z_speed_limit;
  int x_drag;                           ///< x/y/z_drag is friction/drag. It decelerates x_speed toward zero regardless of direction:
                                        ///< 
                                        ///< If moving right (x_speed > 0): x_speed -= x_drag, clamped to 0
                                        ///< If moving left (x_speed < 0): x_speed += x_drag, clamped to 0
  int y_drag;
  int z_drag;
  KKND::MobdAnimation *anim;
  KKND::MobdAnimation *anim_cursor;
  KKND::MobdAnimFrame *anim_current_frame;
  KKND::BoxdCollisionShape *shape;
  KKND::BoxdCollisionHandler *collider;
  int anim_speed;                       ///< anim_speed format: 4.28 fixed-point. Top 4 bits = whole frames to advance per tick.
                                        ///< 0x10000000 (1<<28) = 1 frame/tick. 0x20000000 (2<<28) = 2 frames/tick.
                                        ///< 0x08000000 = 0.5 frames/tick (half speed). 0 = frozen.
  int anim_timer;                       ///< remaining ticks on the anim
  KKND::RenderNode *rn;
  KKND::Task *task;
  KKND::CplcSpawnParams *cplc_spawn_params;
  KKND::CplcEntity *cplc_meta;
  KKND::CplcEntityInViewport *cplc_view;
  void *ctx1;
  KKND::EntityContext ctx2;
  BOOL is_collidable;
  __int16 infantry_damage;
  __int16 vehicle_damage;
  __int16 building_damage;
  __int16 _92_padding;
};

/* 202 */
struct KKND::MobdAnimation
{
  int anim_speed;
  KKND::MobdAnimFrame *frames[1];
};

/* 190 */
enum KKND::SoundId : unsigned __int32
{
  SoundId_Surv_GenericRookie_Move_1 = 0,
  SoundId_1 = 1,
  SoundId_2 = 2,
  SoundId_Explosion = 3,
  SoundId_ProjHit_Rifle = 4,            ///< Rifleman, default
  SoundId_ProjHit_MachineGun_1 = 5,     ///< Dirk Bike, 4x4, Monster Truck
  SoundId_ProjHit_MachineGun_2 = 6,     ///< Autocannon Tank (heavy autocannon)
  SoundId_ProjHit_MachineGun_3 = 7,     ///< Bike & Sidecar (rattling machinegun)
  SoundId_ProjHit_MachineGun_4 = 8,     ///< ATV, Crazy Harry (light autocannon)
  SoundId_Explosion_AircraftCrash = 9,
  SoundId_Explosion_Nuke = 10,
  SoundId_Ricochet_1 = 11,
  SoundId_Ricochet_2 = 12,
  SoundId_Ricochet_3 = 13,
  SoundId_RocketLaunch = 14,
  SoundId_15 = 15,
  SoundId_16 = 16,
  SoundId_ProjHit_Shotgun = 17,         ///< Shotgunner, Dire Wolf
  SoundId_18 = 18,
  SoundId_19 = 19,
  SoundId_20 = 20,
  SoundId_21 = 21,
  SoundId_22 = 22,
  SoundId_23 = 23,
  SoundId_24 = 24,
  SoundId_25 = 25,
  SoundId_26 = 26,
  SoundId_27 = 27,
  SoundId_28 = 28,
  SoundId_29 = 29,
  SoundId_30 = 30,
  SoundId_31 = 31,
  SoundId_Surv_BuildingReady = 32,
  SoundId_Surv_UnitReady = 33,
  SoundId_34 = 34,
  SoundId_35 = 35,
  SoundId_36 = 36,
  SoundId_37 = 37,
  SoundId_38 = 38,
  SoundId_39 = 39,
  SoundId_40 = 40,
  SoundId_41 = 41,
  SoundId_42 = 42,
  SoundId_Surv_GenericRookie_Move_2 = 43,
  SoundId_44 = 44,
  SoundId_Surv_Specialist_Ack = 45,
  SoundId_46 = 46,
  SoundId_Surv_LowOilWarning = 47,
  SoundId_Surv_GenericRookie_Attack_1 = 48,
  SoundId_Surv_GenericRookie_Move_3 = 49,
  SoundId_50 = 50,
  SoundId_51 = 51,
  SoundId_52 = 52,
  SoundId_53 = 53,
  SoundId_Surv_ResearchStarted = 54,
  SoundId_Surv_ResearchComplete_1 = 55,
  SoundId_56 = 56,
  SoundId_57 = 57,
  SoundId_58 = 58,
  SoundId_Surv_GenericRookie_Attack_2 = 59,
  SoundId_Surv_TankerUnderAttack = 60,
  SoundId_61 = 61,
  SoundId_Surv_ResearchComplete_2 = 62,
  SoundId_63 = 63,
  SoundId_64 = 64,
  SoundId_65 = 65,
  SoundId_Surv_AirstrikeReady = 66,
  SoundId_67 = 67,
  SoundId_Surv_GenericRookie_Move_4 = 68,
  SoundId_69 = 69,
  SoundId_70 = 70,
  SoundId_71 = 71,
  SoundId_72 = 72,
  SoundId_Surv_GenericVeteran_Attack = 73,
  SoundId_74 = 74,
  SoundId_75 = 75,
  SoundId_Surv_GenericVeteran_Move_1 = 76,
  SoundId_77 = 77,
  SoundId_78 = 78,
  SoundId_79 = 79,
  SoundId_80 = 80,
  SoundId_81 = 81,
  SoundId_82 = 82,
  SoundId_83 = 83,
  SoundId_84 = 84,
  SoundId_Surv_GenericVeteran_Move_2 = 85,
  SoundId_Flamethrower = 86,
  SoundId_PlasmaProjectile = 87,
  SoundId_88 = 88,
  SoundId_Mute_Bow = 89,
  SoundId_Mute_BowHit = 90,
  SoundId_Mute_GenericRookie_Move_1 = 91,
  SoundId_Mute_GenericRookie_Attack_1 = 92,
  SoundId_93 = 93,
  SoundId_94 = 94,
  SoundId_95 = 95,
  SoundId_Mute_BuildingReady = 96,
  SoundId_97 = 97,
  SoundId_98 = 98,
  SoundId_99 = 99,
  SoundId_100 = 100,
  SoundId_101 = 101,
  SoundId_Mute_ResearchComplete_1 = 102,
  SoundId_103 = 103,
  SoundId_Mute_ResearchComplete_2 = 104,
  SoundId_105 = 105,
  SoundId_106 = 106,
  SoundId_107 = 107,
  SoundId_108 = 108,
  SoundId_109 = 109,
  SoundId_110 = 110,
  SoundId_111 = 111,
  SoundId_112 = 112,
  SoundId_113 = 113,
  SoundId_114 = 114,
  SoundId_115 = 115,
  SoundId_116 = 116,
  SoundId_117 = 117,
  SoundId_118 = 118,
  SoundId_Mute_GenericRookie_Attack_2 = 119,
  SoundId_120 = 120,
  SoundId_121 = 121,
  SoundId_Mute_LowOilWarning = 122,
  SoundId_123 = 123,
  SoundId_Mute_Specialist_Ack = 124,
  SoundId_Mute_UnitReady = 125,
  SoundId_Mute_ResearchStarted = 126,
  SoundId_127 = 127,
  SoundId_128 = 128,
  SoundId_129 = 129,
  SoundId_130 = 130,
  SoundId_Mute_GenericRookie_Move_2 = 131,
  SoundId_Mute_TankerUnderAttack = 132,
  SoundId_Mute_GenericRookie_Move_3 = 133,
  SoundId_134 = 134,
  SoundId_135 = 135,
  SoundId_Mute_AirstrikeReady = 136,
  SoundId_137 = 137,
  SoundId_138 = 138,
  SoundId_139 = 139,
  SoundId_140 = 140,
  SoundId_141 = 141,
  SoundId_142 = 142,
  SoundId_143 = 143,
  SoundId_144 = 144,
  SoundId_145 = 145,
  SoundId_146 = 146,
  SoundId_147 = 147,
  SoundId_Mute_GenericVeteran_Move_1 = 148,
  SoundId_149 = 149,
  SoundId_150 = 150,
  SoundId_151 = 151,
  SoundId_152 = 152,
  SoundId_Mute_GenericVeteran_Attack = 153,
  SoundId_Mute_GenericVeteran_Move_2 = 154,
  SoundId_155 = 155,
  SoundId_156 = 156,
  SoundId_157 = 157,
  SoundId_158 = 158,
  SoundId_Mute_WarMastadont_Ack = 159,
  SoundId_Mute_WarMastadont_Recall = 160,
  SoundId_AcidSpit = 161,
  SoundId_162 = 162,
  SoundId_NukeFallingWhistle = 163,
  SoundId_Mute_DireWolf_Ack = 164,
  SoundId_Mute_DireWolf_Recall = 165,
  SoundId_166 = 166,
  SoundId_167 = 167,
  SoundId_Mute_GiantBeetle_Recall = 168,
  SoundId_Mute_GiantBeetle_Ack = 169,
  SoundId_Mute_MissileCrab_Recall = 170,
  SoundId_Mute_MissileCrab_Ack = 171,
  SoundId_Mute_GiantScorpion_Recall = 172,
  SoundId_Mute_GiantScorpion_Ack = 173,
  SoundId_InfantryDeath1 = 174,
  SoundId_InfantryDeath2 = 175,
  SoundId_InfantryDeath3 = 176,
  SoundId_InfantryDeath4 = 177,
  SoundId_InfantryDeath5 = 178,
  SoundId_FuturisticUnit_Attack_1 = 179,
  SoundId_FuturisticUnit_Attack_2 = 180,
  SoundId_FuturisticUnit_Recall = 181,
  SoundId_FuturisticUnit_Move_1 = 182,
  SoundId_FuturisticUnit_Move_2 = 183,
  SoundId_FuturisticUnit_Move_3 = 184,
  SoundId_FuturisticUnit_Attack_3 = 185,
  SoundId_FuturisticUnit_Move_4 = 186,
  SoundId_GortProjectileHit = 187,
  SoundId_GortProjectile = 188,
  SoundId_MobileBasePlanting = 189,
  SoundId_BulletWhiz = 190,
  SoundId_OilUnloading = 191,
  SoundId_Surv_ScoutRecall = 192,
  SoundId_BuildingPlace = 193,
};

/* 194 */
struct KKND::MobdPoint
{
  int id;
  int x;
  int y;
  int z;
};

/* 195 */
struct KKND::MobdAnimFrame
{
  int x;
  int y;
  int flags;
  KKND::MobdSprtImage *sprt;
  KKND::BoxdCollisionShape *shape;
  KKND::SoundId sound_id;
  KKND::MobdPoint points[];
};

/* 170 */
struct KKND::BoxdCollisionShape
{
  KKND::BoxdAabb *box;
};

/* 205 */
enum KKND::BoxdCollisionAxis : unsigned __int32
{
  BoxdCollisionAxis_NegX = 0,
  BoxdCollisionAxis_PosX = 1,
  BoxdCollisionAxis_NegY = 2,
  BoxdCollisionAxis_PosY = 3,
  BoxdCollisionAxis_NegZ = 4,
  BoxdCollisionAxis_PosZ = 5,
};

/* 204 */
struct KKND::BoxdCollisionHandler
{
  int collides_with_categories;         ///< The collision only proceeds if there is any bit in common between the mover's collides_with_categories (outgoing) and the target shape's category (incoming).
  int category;
  BOOL (__fastcall *mover_response)(KKND::Entity *, KKND::Entity *, KKND::BoxdCollisionAxis, KKND::BoxdAabb *, KKND::BoxdAabb *); ///< 
                                                                                                                                  ///< Two handlers enable asymmetric collision pairs where one side is authoritative:
                                                                                                                                  ///< 
                                                                                                                                  ///< Obstacle-driven (terrain/buildings): Mover has primary=NULL at its root shape. Each obstacle sub-shape defines its own geometry response in secondary. A unit walking into a ramp gets ramp behavior; walking into a wall gets solid push-out — all without the mover knowing what it hit.
                                                                                                                                  ///< 
                                                                                                                                  ///< Mover-driven (cursor/projectile): The mover has primary set. It doesn't matter what the obstacle's secondary says — the mover overrides. The cursor fires a hover event regardless of what kind of entity it's over.
                                                                                                                                  ///< 
                                                                                                                                  ///< Hypothetical dual-response (never used): If both were set, the mover's primary would always win. The obstacle never gets a say. You could imagine a scenario where a special unit type overrides terrain collision (e.g., a flying unit with a custom primary that ignores slopes) — the architecture supports it, but no entry uses both slots simultaneously.
                                                                                                                                  ///< 
                                                                                                                                  ///< The system was clearly designed for a third case — mover-specific override of terrain response. Imagine:
                                                                                                                                  ///< 
                                                                                                                                  ///< - A hovercraft that slides over ramps instead of climbing (override primary to skip Y clamping)
                                                                                                                                  ///< - An ethereal unit that passes through walls but still triggers hover events
                                                                                                                                  ///< - Different weight classes that get pushed differently by the same obstacle
                                                                                                                                  ///< 
                                                                                                                                  ///< None of these shipped, so every game entity uses index 0 (root) with primary=NULL, deferring entirely to the obstacle's secondary. The cursor is the sole example of a mover-driven response. The two-slot design is architectural foresight that simplified to a one-sided dispatch in practice.
  BOOL (__fastcall *obstacle_response)(KKND::Entity *, KKND::Entity *, KKND::BoxdCollisionAxis, KKND::BoxdAabb *, KKND::BoxdAabb *);
};

/* 352 */
enum KKND::RenderNodeFlags : unsigned __int32
{
  RenderNode_PaletteOverride = 0x10000000, ///< use palette override in the render command, see RenderCommandFlags
  RenderNode_Skip = 0x40000000,         ///< skip rendering (invisible)
  RenderNode_Deleted = 0x80000000,      ///< node pending deletion
};

/* 282 */
typedef void (__fastcall *KKND::RenderTransform)(void *renderable, KKND::RenderNode *node);

/* 351 */
enum KKND::RenderCommandFlags : unsigned __int32
{
  RenderCommand_PaletteOverride = 0x10000000,
};

/* 253 */
struct KKND::RenderCommand
{
  KKND::RenderCommandFlags flags;
  KKND::RenderImage *image;             ///< It's actually polymorphic - Scrl and Sprt image objects are assigned to this, both having blitter as the first field so in terms of binary they're compatible
  int _render_command_field_8;
  KKND::RenderViewport *viewport;
  int x;
  int y;
  int z;
  unsigned __int8 *palette_override;    ///< use it when flags & RenderCommand_PaletteOverride
};

/* 221 */
struct KKND::RenderNode
{
  KKND::RenderNode *next;
  KKND::RenderNode *prev;
  KKND::RenderNodeFlags flags;
  void *payload;
  KKND::RenderTransform transform;
  KKND::RenderCommand cmd;
  int _render_node_field_34;
  int _render_node_field_38;
};

/* 176 */
enum KKND::UnitType : unsigned __int32
{
  UnitType_Surv_Rifleman = 0,
  UnitType_Mute_Berserker = 1,
  UnitType_Surv_Flamer = 2,
  UnitType_Mute_Pyromaniac = 3,
  UnitType_Surv_Swat = 4,
  UnitType_Mute_Shotgunner = 5,
  UnitType_Surv_Sapper = 6,
  UnitType_Mute_Rioter = 7,
  UnitType_Surv_ElPresidente = 8,
  UnitType_Mute_KingZog = 9,
  UnitType_Surv_Saboteur = 10,
  UnitType_Mute_Vandal = 11,
  UnitType_Surv_Technician = 12,
  UnitType_Mute_Makanik = 13,
  UnitType_Surv_RpgLauncher = 14,
  UnitType_Mute_Bazooka = 15,
  UnitType_Surv_Sniper = 16,
  UnitType_Mute_CrazyHarry = 17,
  UnitType_Surv_General = 18,
  UnitType_Mute_Warlord = 19,
  UnitType_Surv_Scout = 20,
  UnitType_Surv_MobileDerrick = 21,
  UnitType_Mute_MobileDerrick = 22,
  UnitType_Surv_Tanker = 23,
  UnitType_Mute_Tanker = 24,
  UnitType_TankerConvoy = 25,
  UnitType_Surv_DirkBike = 26,
  UnitType_Mute_DireWolf = 27,
  UnitType_Surv_4x4Pickup = 28,
  UnitType_Mute_BikeAndSidecar = 29,
  UnitType_Surv_Atv = 30,
  UnitType_Mute_MonsterTruck = 31,
  UnitType_Surv_AtvFlamethrower = 32,
  UnitType_Mute_GiantScorpion = 33,
  UnitType_Surv_AnacondaTank = 34,
  UnitType_Mute_WarMastadont = 35,
  UnitType_Surv_BarrageCraft = 36,
  UnitType_Mute_GiantBeetle = 37,
  UnitType_Surv_AutocannonTank = 38,
  UnitType_Mute_MissileCrab = 39,
  UnitType_Surv_MobileOutpost = 40,
  UnitType_Mute_ClanhallWagon = 41,
  UnitType_42 = 42,
  UnitType_Mute_Wasp = 43,
  UnitType_Surv_Bomber = 44,
  UnitType_45 = 45,
  UnitType_Surv_Drillrig = 46,
  UnitType_Mute_Drillrig = 47,
  UnitType_Surv_PowerStation = 48,
  UnitType_Mute_PowerStation = 49,
  UnitType_Surv_DetentionCenter = 50,
  UnitType_Mute_HoldingPens = 51,
  UnitType_Surv_GuardTower = 52,
  UnitType_Mute_MachinegunNest = 53,
  UnitType_SurvCannonTower = 54,
  UnitType_Mute_GrapeshotTower = 55,
  UnitType_Surv_MissileBattery = 56,
  UnitType_Mute_RotaryCannon = 57,
  UnitType_Surv_Outpost = 58,
  UnitType_Mute_Clanhall = 59,
  UnitType_Surv_MachineShop = 60,
  UnitType_Mute_Blacksmith = 61,
  UnitType_Mute_BeastEnclosure = 62,
  UnitType_Surv_RepairBay = 63,
  UnitType_Mute_Menagerie = 64,
  UnitType_Surv_ResearchLab = 65,
  UnitType_Surv_AlchemyHall = 66,
  UnitType_TechBunker = 67,
  UnitType_Hut = 68,
  UnitType_Wall1 = 69,
  UnitType_Wall2 = 70,
  UnitType_Wall3 = 71,
  UnitType_Wall4 = 72,
  UnitType_Gort = 74,
  UnitType_75 = 75,
  UnitType_PlasmaTank = 76,
  UnitType_SentinelDroid = 77,
  UnitType_Mech = 78,
  UnitType_Last = 88,
  UnitType_Invalid = -1,
};

/* 177 */
struct KKND::CplcSpawnParams
{
  KKND::MobdId mobd_id;
  KKND::TaskFn task;
  KKND::TaskKind task_kind;
  BOOL skip_spawning;
  KKND::Entity *entity;
  int _cplc_sawpn_params_field_14;
  int player_side_or_spawn_table_idx;
  int spawn_param;
  KKND::UnitType unit_id;
  int player_num;
  int _cplc_spawn_params_field_28;
  int _cplc_spawn_params_field_2c;
};

/* 183 */
enum KKND::TaskType : __int32
{
  TaskType_DetentionCenter = 2,
  TaskType_HoldingPens = 3,
  TaskType_Clanhall = 4,
  TaskType_Outpost = 5,
  TaskType_MOBD_6_UiSymbols = 6,        ///< Not really a task, only used in CPLC loading as a logic flow control
  TaskType_Mobd_15_MissionOutcomeModal = 9, ///< Not really a task, only used in CPLC loading as a logic flow control
  TaskType_FontItalic = 73,             ///< Not really a task, only used in CPLC loading as a logic flow controlNot really a task, only used in CPLC loading as a logic flow control
  TaskType_KeyboardDispatcher = 75,     ///< 431E60
  TaskType_IngameMenuController = 77,   ///< 47C028
  TaskType_SidebarLoop = 78,            ///< 446ED0 Refreshes sidebar buttons on produciton complete, updates cash widget
  TaskType_MissionOutcomeSequence = 79, ///< 424CE0 Mission outcome sequence (letters appear, sounds plays etc)
  TaskType_CursorLoop = 80,
  TaskType_SidebarTooltip = 81,         ///< 42D030 Sidebar tooltip
  TaskType_Camera = 84,
  TaskType_NewMissionsSelect = 85,      ///< 442BB0 New Missions menu level selection
  TaskType_Reinforcements = 122,        ///< 4269B0
  TaskType_Max = 196,
  TaskType_Invalid = -1,
};

/* 179 */
struct KKND::CplcEntity
{
  KKND::TaskType task_type;
  int x;
  int y;
  int z;
  KKND::CplcEntity *next_x_sorted;
  KKND::CplcEntity *prev_x_sorted;
  KKND::CplcEntity *next_y_sorted;
  KKND::CplcEntity *prev_y_sorted;
  KKND::CplcSpawnParams spawn_params;
};

/* 182 */
struct KKND::CplcEntityInViewport
{
  KKND::CplcEntityInViewport *next;
  KKND::CplcEntityInViewport *prev;
  KKND::Entity *entity;
};

/* 234 */
typedef void (__fastcall *KKND::UnitMode)(KKND::Unit *);

/* 236 */
struct KKND::UnitMobdAnchors
{
  KKND::MobdPoint *turret;
  KKND::MobdPoint *rally;
  KKND::MobdPoint *render;
  KKND::MobdPoint *grid;
  KKND::MobdPoint *_unit_mobd_anchors_10_unused;
  KKND::MobdPoint *dock_point;
};

/* 237 */
/// actually a typedef int, with various enum values are simply #defines - there are units with 8 and 16 orientations (potentially more) both use this type for their orientation holding variables
enum KKND::MobdOrientation : unsigned __int32
{
  MobdOrientation_N = 0,
  MobdOrientation_NNE = 16,
  MobdOrientation_NE = 32,
  MobdOrientation_E = 64,
  MobdOrientation_SE = 96,
  MobdOrientation_SSE = 112,
  MobdOrientation_S = 128,
  MobdOrientation_SSW = 144,
  MobdOrientation_SW = 160,
  MobdOrientation_W = 192,
  MobdOrientation_NW = 224,
  MobdOrientation_NNW = 240,
};

/* 238 */
enum KKND::Veterancy : unsigned __int32
{
  Veterancy_Rookie = 0x0,
  Veterancy_Mature = 0x1,
  Veterancy_Veteran = 0x2,
};

/* 239 */
typedef __int32 fixed;

/* 240 */
enum KKND::UnitTilePosition : unsigned __int32
{
  UnitPosition_Slot0 = 0,               ///< Always 0 for all non-infantry units
  UnitPosition_Slot1 = 1,
  UnitPosition_Slot2 = 2,
  UnitPosition_Slot3 = 3,
  UnitPosition_Slot4 = 4,
  UnitTilePosition_Invalid = 5,
  UnitTilePosition_BuildingPlacement = 64,
};

/* 241 */
enum KKND::UnitOrderType : unsigned __int32
{
  UnitOrder_Idle = 0,
  UnitOrder_Move = 1,
  UnitOrder_Attack = 2,
  UnitOrder_RepairInfiltrate = 3,
  UnitOrder_Retreat = 4,
  UnitOrder_Escort = 5,
  UnitOrder_TankerConvoyNextCheckpoint = 6,
  UnitOrder_TankerDock = 7,
  UnitOrder_AttackMove = 8,
  UnitOrder_GuardArea = 9,
  UnitOrder_RepairBayDock = 10,
  UnitOrder_ReturnToPosition = 11,      ///< after ATTACK: target destroyed/lost during attack → return to home
                                        ///< after RETREAT: retreat completed → return to home
};

/* 247 */
enum KKND::UnitPathFlags : unsigned __int32
{
  UnitPathFlags_ApproachingWaypoint = 0x1, ///< close to target, skip rotation; cleared on path reset
  UnitPathFlags_CwPassThrough = 0x2,    ///< close to target, skip rotation; cleared on path reset
  UnitPathFlags_CcwPassThrough = 0x4,   ///< allows CCW trace to skip an occupied tile; cleared on path reset
  UnitPathFlags_CwObstacleScan = 0x8,   ///< scanning CW around obstacle
  UnitPathFlags_CcwObstacleScan = 0x10, ///< scanning CCW around obstacle
  UnitPathFlags_ScansDiverged = 0x20,   ///< CW/CCW scans found different tiles
  UnitPathFlags_PassThroughFriendly = 0x40, ///< set when blocked by slower same-direction units; cleared on nudge
  UnitPathFlags_AiWanderer = 0x80,      ///< created from prison/bunker/outpost
  UnitPathFlags_AiSquadlessRoamer = 0x200,
};

/* 225 */
/// Direction is actually a typedef int, but some well-known values like Direction8 (for sprites) and Direction16 (for pathing) are commonly used
enum KKND::Direction
{
  Direction_N = 0,
  Direction_NE = 1,
  Direction_E = 2,
  Direction_SE = 3,
  Direction_S = 4,
  Direction_SW = 5,
  Direction_W = 6,
  Direction_NW = 7,
  Direction_Invalid = -1,
};

/* 250 */
enum KKND::BoxdRaycastResult : unsigned __int32
{
  BoxdRaycastResult_ClearWithWaypoints = 0,
  BoxdRaycastResult_UnitObstacle = 1,
  BoxdRaycastResult_NoWaypoints = 2,
  BoxdRaycastResult_TerrainObstacle = 3,
  BoxdRaycastResult_ClearStraightLine = 4,
  BoxdRaycastResult_InvalidState = 5,
};

/* 249 */
/// Scan-phase pathing working state.
struct KKND::UnitScanPhaseNav
{
  int ray_stack;                        ///< Stack index into raycast results
  int cw_scan_x;                        ///< CW scan probe tile position. Also used directly for movement target when scan finds passable tile. Also used outside scanning as a general "current target tile" — written when finding a blocked unit, and for blocked-unit speed comparison tiles.
  int cw_scan_y;
  int ccw_scan_x;
  int ccw_scan_y;
  KKND::Direction cw_heading;           ///< Approach direction rotated 90° CW to step around obstacles
  KKND::Direction ccw_heading;          ///< Approach direction rotated 90° CCW to step around obstacles
  int first_clear_tile_x;               ///< Blocked-tile fallback target. This is the first free tile hit by the ray that has no prior passable tiles — meaning the ray started directly inside an obstacle and this is the first clear tile found. Used as fallback waypoint in raycast results 2 and 5 — replaces order_next_waypoint when primary path data is incomplete.
                                        ///< Initialized to (0,0) every raycast. Nonzero check = "was a fallback found?"
  int first_clear_tile_y;
  KKND::BoxdRaycastResult ray_result;
  int disperse_timer;                   ///< Temporarily treat partially occupied tiles as impassible to avoid congestion
  int push_through_timer;               ///< Temporarily treat partially occupied tiles as clear to help the unit push through (e.g a freshly created unit leaving its production building)
};

/* 350 */
enum KKND::BlitterMode : unsigned __int32
{
  BlitterMode_Render = 0x0,
  BlitterMode_GetWidth = 0x1,
  BlitterMode_GetHeight = 0x2,
};

/* 192 */
/// mode =
/// 0: render
/// 1: query width
/// 2: query height
typedef int (__fastcall *KKND::Blitter)(KKND::RenderCommand *cmd, KKND::BlitterMode mode);

/* 193 */
struct KKND::MobdSprtImage
{
  KKND::Blitter blitter;
  int flags;                            ///< &1 = flip horizontally
  KKND::MobdImageData *bitmap;
};

/* 220 */
struct KKND::Unit
{
  KKND::Unit *next;
  KKND::Unit *prev;
  KKND::Unit *locked_target;            ///< current active target
  KKND::Task *task;
  KKND::UnitType type;
  int player_num;
  KKND::UnitStats *stats;
  KKND::Turret *turret;
  void *state;
  void *ai_node_per_side[7];
  KKND::UnitMode mode;
  KKND::UnitMode mode_idle;
  KKND::UnitMode mode_arrive;
  KKND::UnitMode mode_attacked;
  KKND::UnitMode mode_return;
  KKND::UnitMode mode_turn_return;
  KKND::MessageHandler message_handler;
  KKND::Entity *entity;
  KKND::UnitMobdAnchors mobd_anchors;
  int _unit_field_78_unused;
  KKND::MobdOrientation orientation;
  int _unit_field_80_unused;
  int _unit_field_84_unused;
  KKND::MobdOrientation target_orientation;
  BOOL destroyed;
  int hitpoints;
  int experience;
  KKND::Veterancy veterancy;
  fixed hp_regen_accumulator;           ///< classic fixed-point fractional accumulation: accumulator overflows by 256 → gain 1 HP. Each idle tick: accumulator += increment. When high byte changes ((new ^ old) & 0xFFFFFF00), integer HP crossed → hitpoints++
  fixed hp_regen_rate;
  KKND::UnitTilePosition tile_position;
  int map_x;
  int map_y;
  int order_next_waypoint_x;
  int order_next_waypoint_y;
  int order_starting_x;
  int order_starting_y;
  int base_anim_speed;                  ///< needs saving because every frame there's a random nudge to every unit's anim so that a group of units don't turn/move inunnatural lockstep
  int path_next_tile_x;
  int path_next_tile_y;
  int path_next_waypoint_tile_x;
  int path_next_waypoint_tile_y;
  int path_scan_direction;              ///< Direction flag for wall-following. Set to 1 or 0 depending which clockwise/counterclockwise scan found shorter path. Checked as: not clockwise ? -1 : 1 (scan direction multiplier), and if clockwise → orientation +64, else -64.
                                        ///< → path_scan_direction (0 = counterclockwise, 1 = clockwise)
  int path_scan_orientation;            ///< Orientation for pathfinding obstacle avoidance. Set from (orientation ± 64) & 0xE0 — perpendicular to current facing. Used as (path_scan_direction >> 5) to get 3-bit direction index for tile neighbor scanning.
  KKND::UnitOrderType order;
  KKND::Unit *order_target;             ///< Primary target of current order (attack, repair, dock target)
  KKND::Unit *opportunity_target;       ///< Attack-move opportunity target or when enemy approaches an idle unit
  KKND::Unit *escort_target;
  int order_target_id;
  int opportunity_target_id;
  int active_target_id;
  int escort_target_id;
  int order_target_x;
  int order_target_y;
  KKND::UnitEscortNode *escort_list_head;
  KKND::UnitEscortNode *escort_list_tail;
  int _unit_field_10C_unused;
  int _unit_field_110_unused;
  int _unit_field_114_unused;
  KKND::Unit *last_attacker;
  KKND::UnitUnion1 _u1;
  KKND::UnitPathFlags path_flags;
  int multi_purpose_field_1;            ///< 1. Infantry/Vehicle — Obstacle wait countdown (value: 60)
                                        ///<     Set to 60 when unit hits blocked tile (another unit in way)
                                        ///<     Decremented each frame in entity_mode_417FC0, entity_mode_4181B0
                                        ///<     When reaches 0 → give up waiting, repath
                                        ///<     Meaning: blocked_wait_timer or obstacle_patience_counter
                                        ///< 
                                        ///< 2. Infantry/Vehicle — Pathfinding step counter (value: 0, incremented)
                                        ///<     Set to 0 at start of obstacle-scan movement (Infantry.cpp:3193)
                                        ///<     Incremented each step in entity_mode_417A20 — compared against base_anim_speed
                                        ///<     Meaning: path_scan_step_count — how many scan steps taken around obstacle
                                        ///< 
                                        ///< 3. Repair bay — Docking animation countdown (value: 100)
                                        ///<     Set to 100 when entering/leaving repair bay (Infantry.cpp:4263)
                                        ///<     Decremented in entity_mode_419180_in_repairbay / entity_mode_418E90_leaving_repair_bay
                                        ///<     When ≤ 0 → done moving into/out of bay
                                        ///<     Meaning: repair_bay_move_timer
                                        ///< 
                                        ///< 4. Repair sprite — Active repair script count (value: 1, incremented)
                                        ///<     In script_4188F0 (repair coroutine): incremented when repair sprite spawned, decremented on terminate
                                        ///<     Used as refcount for repair overlay sprites
                                        ///<     Meaning: repair_sprite_refcount
                                        ///< 
                                        ///< 5. Aircraft — Bomb run pass counter (value: 2, decremented)
                                        ///<     Set to 2 for bomber on spawn (Aircraft.cpp:165)
                                        ///<     Decremented per bombing pass; -1 = return to base, -2 = wait
                                        ///<     Meaning: bombing_passes_remaining
                                        ///< 
                                        ///< 6. Tech bunker / Hut — Detection radius (value: 24576)
                                        ///<     Set to 24576 (= 3 tiles × 8192) on creation (Detenshn.cpp:438)
                                        ///<     Passed to entity_find_any_entity_in_radius() as distance parameter
                                        ///<     Then overwritten with player_side of found entity
                                        ///<     Meaning: detect_radius → then captured_by_side
                                        ///< 
                                        ///< 7. Scout — Detection radius (value: 76800)
                                        ///<     Set to 76800 in UNIT_Handler_Scout (Mission.cpp:936)
                                        ///<     Same pattern as tech bunker — passed to entity_find_player_entity_in_radius()
                                        ///<     Meaning: scout_detect_radius
  int cplc_spawn_param;                 ///< 1. Prison/Bunker — Spawn queue count (value: 0–10)
                                        ///<     Set to 10 on prison death → spawns units one-by-one
                                        ///<     Decremented per spawn; when 0 → done spawning
                                        ///<     Also: index into surv_prison_spawns_table[] / mute_prison_spawns_table[] (counts down from 10)
                                        ///<     Meaning: spawn_queue_remaining
                                        ///< 
                                        ///< 2. Tech bunker — Spawn type / loot table index (value: 0–10, 9 = random)
                                        ///<     Set from level data (param_1C) or 9 (= pick random) in entity_407690_techbunker_spawn
                                        ///<     Values 0–3: unit from techbunker_spawns_table[], 4: tanker, 5: +5000 resources, 6: +1000 resources
                                        ///<     Value 10: special — spawn El Presidente then set to 5
                                        ///<     Meaning: techbunker_spawn_type
                                        ///< 
                                        ///< 3. Hut — Variant / orientation selector (value: 0–4)
                                        ///<     Read on creation in UNIT_Handler_Hut, used in switch to pick anim frame (0/16/32/48/64)
                                        ///<     Comes from level data. Never modified after init.
                                        ///<     Meaning: hut_variant
                                        ///< 
                                        ///< 4. Aircraft — Fire cooldown timer (value: 15, decremented)
                                        ///<     Set to 15 after firing projectile in aircraft attack mode (Aircraft.cpp:341)
                                        ///<     Decremented each tick; when 0 → can fire again
                                        ///<     Also set to 0 when bombing pass ends (Aircraft.cpp:395)
                                        ///<     Meaning: fire_cooldown
                                        ///< 
                                        ///< 5. Building/DrillRig/Tanker — "Under attack" voice line cooldown (value: 1000–2000)
                                        ///<     Set to 1500 (drillrig), 2000 (building), 1000 (tanker) when attacked
                                        ///<     Decremented in unit handler each tick
                                        ///<     When 0 → can play "under attack" sound again
                                        ///<     Prevents voice line spam
                                        ///<     Meaning: attacked_voice_cooldown
                                        ///< 
                                        ///< 6. AI Wanderer — Wanderer tracking timer (value: 2, decremented)
                                        ///<     Set to 2 when unit added to AI wanderer list (EnemyAI.cpp:3968)
                                        ///<     Decremented in AI tick; odd values trigger distance check; when 0 → remove from wanderer list
                                        ///<     Meaning: ai_wanderer_ttl
                                        ///< 
                                        ///< 7. Entity spawn — "Spawned from building" flag (value: from level data or 2)
                                        ///<     On creation via EntityFactory::Create: set from param_1C or hardcoded 2
                                        ///<     If nonzero → path_flags |= PATHFIND_SPAWNED_FROM_BUILDING
                                        ///<     Meaning: spawn_source_param (how many tiles to walk out of building)
                                        ///< 
                                        ///< 8. Scout — Discovery delay multiplier (value: 60)
                                        ///<     Set to 60 in UNIT_Handler_Scout (Mission.cpp:935)
                                        ///<     Stored into _134 on discovery: _134 = _12C
                                        ///<     Meaning: scout_discovery_delay
                                        ///< 
                                        ///< 9. Mission objective — Building alive flag (value: 0 or nonzero)
                                        ///<     Checked == 0 to mean "building destroyed" in mission win/fail conditions (Mission.cpp:1084)
                                        ///<     Meaning: is_destroyed (0 = dead, nonzero = alive/cooldown active)
  int unit_id;
  int multi_purpose_field_3;            ///< 1. Mobile Outpost/Clanhall — Saved unit_id before planting (UnitType value)
                                        ///<     In entity_4279E0_mobile_outpost_clanhall_wagon_plant: _134 = unit_type before swapping to outpost/clanhall unit_type
                                        ///<     If plant fails → unit_type = _134 to restore original
                                        ///<     After plant succeeds → unit_type = _134 in entity_427C30 to spawn mobile unit as building
                                        ///<     Meaning: saved_unit_id
                                        ///< 
                                        ///< 2. Infantry/Vehicle/General — "New order" immunity timer (value: 600, decremented)
                                        ///<     Set to 600 whenever unit receives a new order (move, attack, escort, guard area, repair, etc.)
                                        ///<     Decremented each tick in unit handler
                                        ///<     While nonzero → blocks opportunity fire (!_134 && can_fire_on_entity check in Infantry.cpp:412)
                                        ///<     Also blocks entity_mode_405690 tanker convoy idle transition
                                        ///<     Meaning: order_cooldown — prevents unit from immediately re-engaging opportunity targets after receiving new orders. Gives time to start executing order before getting distracted.
                                        ///< 
                                        ///< 3. Aircraft — Ammo / shots remaining (value: 1, decremented)
                                        ///<     Set to 1 when bomber starts attack pass (Aircraft.cpp:396)
                                        ///<     Checked nonzero → fire projectile + decrement (Aircraft.cpp:311-342)
                                        ///<     When 0 → stop firing, transition to next mode
                                        ///<     Meaning: ammo_remaining
                                        ///< 
                                        ///< 4. Tech bunker — Spawn delay timer (value: ~28800–54000 or 5)
                                        ///<     In multiplayer: _134 = rand() % 25200 + 28800 (~48–90 second delay at 10fps) (Detenshn.cpp:409)
                                        ///<     In singleplayer: _134 = 5 (almost instant)
                                        ///<     Decremented in entity_mode_407950_techbunker_spawn_generic; when ≤ 0 → show turret, enable detection
                                        ///<     Meaning: techbunker_activation_delay
                                        ///< 
                                        ///< 5. Scout — Discovery delay countdown (value: copied from _12C, e.g. 60)
                                        ///<     In entity_mode_425920_scout: _134 = _12C (scout discovery delay)
                                        ///<     While nonzero → scout stays dormant, waiting to be discovered
                                        ///<     Meaning: scout_dormant_timer
                                        ///< 
                                        ///< 6. Loaded unit — Reset to 0 on load
                                        ///<     When loading saved game entity, if entity has no cplc meta: _134 = 0; veterancy = 0; — initialization for "fresh" spawn from save
                                        ///<     Meaning: just zeroed out as part of init
  int path_scan_origin_tile_x;          ///< Sprite tile X at scan start. Exit condition: if scan returns within 1 tile of origin → done
  int path_scan_origin_tile_y;
  int path_scan_iteration;              ///< Monotonic counter, 0→max. Incremented each loop step. Also used in attack-move scan
  int path_scan_max_iterations;         ///< = 2 × initial manhattan distance. Failsafe loop limit
  int path_scan_best_distance;          ///< Lowest manhattan distance to target found so far. Updated when better candidate discovered
  int path_scan_best_tile_x;            ///< X of best waypoint candidate. On scan completion → copied to path_next_waypoint_tile_x
  int path_scan_best_tile_y;
  int path_scan_best_iteration;         ///< Iteration # when best candidate found. After scan: base_anim_speed = this + 1 (sync animation to path length)
  int path_scan_cur_distance;           ///< Scratch — manhattan distance of current candidate being evaluated. Overwritten each iteration
  int ray_exit_map_xs[10];              ///< First free tile after each obstacle zone — the actual waypoint target
  int ray_exit_map_ys[10];
  int ray_unit_obstacle_map_xs[10];     ///< Last unit obstacle tile before exit — gives scan approach direction
  int ray_unit_obstacle_map_ys[10];
  int ray_terrain_obstacle_xs[10];      ///< Last terrain-wall (class-0) tile — fallback approach for when entity obstacles are unreliable198.
  int ray_terrain_obstacle_ys[10];
  KKND::UnitScanPhaseNav scan_pathing;  ///< Pathing works in two phases:
                                        ///<     1. Raycast phase (0041B970 BOXD_pathing_bresenham_raycast) — Cast Bresenham ray from unit to target. Records obstacle boundaries into ray_exit_* / ray_obstacle_* / ray_terrain_* arrays, indexed by ray_stack.
                                        ///<     2. Obstacle-scan phase (e.g unit_mode_416060, unit_mode_attack_move) — fine-grained local navigation: for each waypoint, run CW + CCW wall-following scans around the obstacle. Two scan probes advance independently, each tracking its current tile position.
                                        ///< 
                                        ///< This is actually a union and some of these fields are used for
                                        ///< different purposes e.g to save x/y speed values depending on context
                                        ///< 
  KKND::Unit *nav_obstacle;             ///< Pointer to blocking/obstructing entity found during pathfinding tile checks.
  int nav_obstacle_id;
  KKND::MobdSprtImage overlay_sprt;     ///< Overlay sprite (selection, healthbar)
  KKND::RenderNode *overlay_rn;         ///< Render node that controls ovelay sprite rendering (not sure why the sprite is held separately since render node already owns the image unit->overlay->image = &unit->overlay_sprt)
  unsigned __int8 control_groups[8];
  __int16 idle_fidget_timer;
  __int16 last_stuck_tile_x;
  __int16 last_stuck_tile_y;
  __int16 stuck_timer;                  ///< If unit is stuck in the same tile , stuck timer accumulates and unit sleeps for increasing amount of time waiting to get unstuck
  __int16 next_order;                   ///< New interrupting order received mid-action: 0=idle, 1=move, 2=attack
  __int16 _unit_field_2A6;
  KKND::Unit *next_order_target;
  int next_order_target_id;
  int next_order_target_x;
  int next_order_target_y;
};

/* 370 */
struct KKND::MenuWidget
{
  KKND::MenuWidget *next;
  KKND::MenuWidget *prev;
  KKND::Entity *entity;
  int flags;
};

/* 206 */
enum KKND::BoxdCollisionType : unsigned __int32
{
  BoxdCollisionType_Unit = 0,
  BoxdCollisionType_Solid = 1,
  BoxdCollisionType_Floor = 2,
  BoxdCollisionType_RampLtr = 3,
  BoxdCollisionType_RampRtl = 4,
  BoxdCollisionType_SlopeLeft = 5,
  BoxdCollisionType_SlopeRight = 6,
  BoxdCollisionType_FloorAlt = 7,
  BoxdCollisionType_Skip = -3,
  BoxdCollisionType_OffGrid = -2,
  BoxdCollisionType_Always = -1,
};

/* 169 */
struct KKND::BoxdAabb
{
  KKND::BoxdCollisionType type;
  int min_x;
  int min_y;
  int min_z;
  int max_x;
  int max_y;
};

/* 349 */
struct KKND::RenderImage
{
  KKND::Blitter blitter;                ///< It's actually polymorphic - Scrl and Sprt image objects are assigned to this, both having blitter as the first field so in terms of binary they're compatible
};

/* 208 */
struct KKND::RenderViewport
{
  KKND::RenderViewport *next;
  KKND::RenderViewport *prev;
  int flags;
  int brightness;                       ///< Seemingly uses 8.23 format - brightness factor actually used is (brightness >> 23)
  int clip_x;
  int clip_y;
  int clip_w;
  int clip_h;
  int _render_viewport_20;
};

/* 285 */
enum KKND::UnitSize : unsigned __int32
{
  UnitSize_None = 0,
  UnitSize_Regular = 128,               ///< 1 tile unit
  UnitSize_Small = 512,                 ///< sub-tile unit (infantry) - up to 5 in one tile
  UnitSize_XL = 4096,                   ///< 2x2 tiles unit
};

/* 229 */
enum KKND::Race : unsigned __int32
{
  Race_Survivors = 1,
  Race_Evolved = 2,
};

/* 227 */
struct KKND::UnitStats
{
  KKND::MobdId mobd_id;
  KKND::TaskFn task_fn;
  const char *name;
  int cost;
  int hitpoints;
  int speed;
  int reload2_time;
  int turning_speed;
  int view_range;
  int firing_range;
  int accuracy;
  BOOL can_crush;
  BOOL is_infantry;
  ptrdiff_t mobd_lookup_offset_attack;  ///< -1 for turreted units
  ptrdiff_t mobd_lookup_offset_move;
  ptrdiff_t mobd_lookup_offset_idle;
  ptrdiff_t mobd_lookup_offset_4;       ///< damaged_buildings?
  KKND::UnitAttachment *attachment;
  KKND::UnitProjectileType *projectile;
  KKND::UnitSize size;                  ///< map tile size
                                        ///< 
                                        ///< 4096: XL (2x2 tiles)
                                        ///< autocannon, missile crab, mobile outposts, gort, plasma tank
                                        ///< 
                                        ///< 512: 1/5th of a tile
                                        ///< infantry
                                        ///< 
                                        ///< 128: exactly 1 tile
                                        ///< vehicles, buildings,
                                        ///< tech bunker, sentinel droid
                                        ///< gorn, tree, hut
  KKND::Race race;
  int ai_threat_weight;
  int ai_strategic_value;
  KKND::UnitType factory;
  int production_time;
};

/* 283 */
typedef void (__fastcall *KKND::TurretMode)(KKND::Turret *);

/* 228 */
struct KKND::Turret
{
  KKND::Task *task;
  KKND::Entity *entity;
  KKND::Unit *parent;
  KKND::Unit *target;
  KKND::TurretMode mode;
  ptrdiff_t current_mobd_frame;
  int reload_timer;
  int volley_remaining;
  int volley_reload_time;
  KKND::MobdPoint *projectile_spawn_anchor;
  KKND::UnitAttachment *attachment;
  int _turret_field_2C_unused;
  int target_unit_id;
  int _turret_field_34_unused;
};

/* 231 */
struct KKND::UnitEscortNode
{
  KKND::UnitEscortNode *next;
  KKND::UnitEscortNode *prev;
  KKND::Unit *escort;
};

/* 246 */
struct KKND::Vec2i
{
  fixed x;
  fixed y;
};

/* 244 */
union KKND::UnitUnion1
{
  KKND::Vec2i infantry_return;
  KKND::OilPatch *oil_patch;
};

/* 191 */
struct __unaligned __declspec(align(1)) KKND::MobdImageData
{
  int width;
  int height;
  unsigned __int8 format;               ///< pixel format: 0=raw, 2=RLE compressed
                                        ///< raw: one byte per pixel (palette index). Zero bytes are transparent
  unsigned __int8 pixels[];
};

/* 232 */
struct KKND::UnitAttachment
{
  KKND::MobdId mobd_id;
  void (__fastcall *mode_attach)(KKND::Task *task);
  int rotation_speed;
  int reload_time;                ///< fire -> fire delay -> fire -> fire delay -> ... end of volley -> wait reload_time
  int reload2_time;
  int volley_size;
  ptrdiff_t mobd_lookup_offset_idle;
  ptrdiff_t mobd_lookup_offset_attack;
  KKND::UnitProjectileType *projectile_type;
  int _unit_attachment_field_24;
};

/* 233 */
struct KKND::UnitProjectileType
{
  KKND::MobdId mobd_id;
  void (__cdecl *task_fn)(KKND::Task *task);
  ptrdiff_t mobd_lookup_offset_travel;
  ptrdiff_t mobd_lookup_offset_impact;
  int speed;
  int damage_to_infantry;
  int damage_to_vehicles;
  int damage_to_buildings;
  int radius;                     ///< 32 for most, 128 for bombers - radius/8 tiles around the impact
  int _projectile_type_field_24;
};

/* 219 */
struct KKND::OilPatch
{
  KKND::OilPatch *next;
  KKND::OilPatch *prev;
  KKND::Entity *entity;
  int amount;
  KKND::Unit *drillrig;
  int drillrig_unit_id;
};

/* 167 */
struct KKND::LevelHunkSection
{
  char name[4];
  void *ptr;
};

/* 168 */
struct KKND::LevelHunk
{
  KKND::LevelHunkSection *sections[];
};

/* 171 */
struct KKND::BoxdSpatialHashEntry
{
  KKND::BoxdCollisionShape *shape;
  KKND::Entity *entity;
  KKND::BoxdSpatialHashEntry *next;
};

/* 173 */
struct BoxdGrid
{
  int max_collision_buckets;
  int world_to_tile_x;
  int world_to_tile_y;
  int num_tiles_x;
  int num_tiles_y;
  KKND::BoxdAabb *tiles[1];
};

/* 174 */
struct KKND::LevelBoxd
{
  BoxdGrid *grid[1];
};

/* 180 */
struct KKND::LevelCplcSurface
{
  int size;
  KKND::CplcEntity *next_x_sorted;
  KKND::CplcEntity *prev_x_sorted;
  KKND::CplcEntity *next_y_sorted;
  KKND::CplcEntity *prev_y_sorted;
};

/* 181 */
struct KKND::LevelCplc
{
  KKND::LevelCplcSurface *layers;
};

/* 184 */
struct KKND::MapdCamera
{
  int x;
  int y;
  int z;
};

/* 185 */
struct __unaligned KKND::MapdScrlImageTile
{
  int flags;
  unsigned __int8 pixels[1];
};

/* 186 */
struct KKND::MapdScrlImage
{
  KKND::Blitter renderer;
  int tile_x_size;
  int tile_y_size;
  int num_x_tiles;
  int num_y_tiles;
  KKND::MapdScrlImageTile *tiles[16];
};

/* 200 */
struct KKND::PaletteEntry
{
  unsigned __int8 r;
  unsigned __int8 g;
  unsigned __int8 b;
  unsigned __int8 flags;
};

/* 187 */
struct KKND::LevelMapdSurface
{
  int num_images;
  KKND::MapdScrlImage *images[1];
  int num_palette_entries;
  KKND::PaletteEntry palette[1];
};

/* 188 */
struct KKND::LevelMapd
{
  KKND::LevelMapdSurface *layers;
};

/* 189 */
struct KKND::MapdRenderNode
{
  KKND::MapdRenderNode *next;
  KKND::MapdRenderNode *prev;
  KKND::RenderNode *rn;
  KKND::MapdScrlImage *scrl;
  int z;
};

/* 196 */
struct KKND::LevelMobdSurface
{
  KKND::MobdAnimFrame frames[1];
};

/* 197 */
struct KKND::LevelMobd
{
  KKND::LevelMobdSurface *layers[1];
};

/* 199 */
enum KKND::MenuId : unsigned __int32
{
  MenuId_Main = 0,
  MenuId_Multiplayer = 1,
  MenuId_NewCampaign = 2,
  MenuId_3 = 3,
  MenuId_IpxSetup = 4,
  MenuId_Serial = 5,
  MenuId_Modem = 6,
  MenuId_HostGame = 7,
  MenuId_JoinGame = 8,
  MenuId_AddModem = 9,
  MenuId_ModemPhonebook = 10,
  MenuId_PlayMission = 11,
  MenuId_Credits = 12,
  MenuId_MissionComplete = 13,
  MenuId_NewMissions = 14,
  MenuId_Kaos = 15,
};

/* 201 */
struct KKND::Palette
{
  KKND::PaletteEntry entries[256];
};

/* 209 */
struct KKND::Coroutine
{
  KKND::Coroutine *yield_to;
  int *context;
  void *stack;
  KKND::Coroutine *next;
};

/* 210 */
struct KKND::RenderBatch
{
  KKND::RenderNode *next;
  KKND::RenderNode *prev;
};

/* 212 */
/// The game does NOT use a traditional switch-based game state dispatcher. Instead, three nested `do/while` loops share a single guard variable `g_game_loop`. Value `0` means "keep ticking current loop.
enum KKND::GameLoop : unsigned __int32
{
  GameLoop_Continue = 0,
  GameLoop_NextMission = 1,
  GameLoop_EndMission = 2,
  GameLoop_PreviousScreen = 3,
};

/* 213 */
enum KKND::MissionOutcome : unsigned __int32
{
  MissionOutcome_NotStarted = 0x0,
  MissionOutcome_Defeat = 0x1,
  MissionOutcome_Victory = 0x2,
  MissionOutcome_Abandoned = 0x6,
};

/* 214 */
enum KKND::LevelId : unsigned __int32
{
  LevelId_Surv_01_TheNextGeneration = 0,
  LevelId_Surv_02_BuildAnOutpost = 1,
  LevelId_Surv_03_WithstandTheRaidingParty = 2,
  LevelId_Surv_04_RescueTheScout = 3,
  LevelId_Surv_05_TollGate = 4,
  LevelId_Surv_06_ExterminateTheVillage = 5,
  LevelId_Surv_07_ProtectTheConvoy = 6,
  LevelId_Surv_08_BackToTheBeach = 7,
  LevelId_Surv_09_RescueTheGeneral = 8,
  LevelId_Surv_10_OccupationForce = 9,
  LevelId_Surv_11_GiveMeLiberty = 10,
  LevelId_Surv_12_SurgicalStrike = 11,
  LevelId_Surv_13_HoldTheBridge = 12,
  LevelId_Surv_14_TheRoutRetreat = 13,
  LevelId_Surv_15_Beseiged = 14,
  LevelId_Mute_01_TheReturnOfTheSlugs = 15,
  LevelId_Mute_02_RepelThemRepelThem = 16,
  LevelId_Mute_03_BubblinCrude = 17,
  LevelId_Mute_04_RaidTheFort = 18,
  LevelId_Mute_05_Ambush = 19,
  LevelId_Mute_06_Seige = 20,
  LevelId_Mute_07_ReleaseThePrisoners = 21,
  LevelId_Mute_08_SmashTheConvoy = 22,
  LevelId_Mute_09_FightForTerritory = 23,
  LevelId_Mute_10_ItsTheEndOfTheWorld = 24,
  LevelId_Mute_11_CloseEncounters = 25,
  LevelId_Mute_12_BigAttach = 26,
  LevelId_Mute_13_BattleForTheIslands = 27,
  LevelId_Mute_14_TheFinalAssault = 28,
  LevelId_Mute_15_CounterAttack = 29,
  LevelId_Surv_16 = 48,
  LevelId_Surv_17 = 49,
  LevelId_Surv_18 = 50,
  LevelId_Surv_19 = 51,
  LevelId_Surv_20 = 52,
  LevelId_Surv_21 = 53,
  LevelId_Surv_22 = 54,
  LevelId_Surv_23 = 55,
  LevelId_Surv_24 = 56,
  LevelId_Surv_25 = 57,
  LevelId_Mute_16 = 58,
  LevelId_Mute_17 = 59,
  LevelId_Mute_18 = 60,
  LevelId_Mute_19 = 61,
  LevelId_Mute_20 = 62,
  LevelId_Mute_21 = 63,
  LevelId_Mute_22 = 64,
  LevelId_Mute_23 = 65,
  LevelId_Mute_24 = 66,
  LevelId_Mute_25 = 67,
  LevelId_Invalid = -1,
};

/* 366 */
enum KKND::MissionVictoryConditionBits : unsigned __int32
{
  VictoryCondition_KrushKillnDestroy = 0x1, ///< Destroy all enemy buildings
  VictoryCondition_Timed = 0x2,         ///< Stay alive for 108000 ticks (30min at 60tps)
  VictoryCondition_Economic = 0x4,      ///< Accumulate 5000 and keep Power Plant alive
  VictoryCondition_Survive = 0x8,       ///< Keep own buildings alive
  VictoryCondition_300 = 0x300,         ///< Always set; cut feature - must have been something e.g 0x100 keep unuts 0x200 keep buildings
};

/* 321 */
struct KKND::AiAirstrikeConfig
{
  __int16 count;
  __int16 interval;
  unsigned __int16 rng_interval_min;
  unsigned __int16 rng_interval_max;
};

/* 215 */
struct KKND::LevelDesc
{
  const char *lvl_filename;
  const char *wav_filename;
  const char *vbc_filename;
  __int16 player_starting_cash;
  __int16 ai_starting_cash;
  __int16 superlvl_ai_cash;
  __int16 superlvl_ai_cost_reduction;
  __int16 max_tech_level;
  __int16 _level_desc_field_16;
  unsigned int disabled_units_mask;
  KKND::MissionVictoryConditionBits victory_condition;
  KKND::AiAirstrikeConfig superlvl_ai_airstrike_count_enemy;
  KKND::AiAirstrikeConfig superlvl_ai_airstrike_count_ally;
};

/* 216 */
struct KKND::MissionCashTable
{
  int cash[7];
};

/* 217 */
struct KKND::MissionDiplomacyTable
{
  BOOL is_ally[7][7];
};

/* 218 */
struct KKND::OilPatchSaveStruct
{
  __int32 amount;
  __int32 x;
  __int32 y;
  __int32 drillrig_entity_id;
};

/* 223 */
enum KKND::TerrainTileFlags1 : unsigned __int8
{
  TerrainTileFlags1_None = 0x0,
  TerrainTileFlags1_InfantrySlot0 = 0x1, ///< Infantry slot bitmask. Each bit = 1 of 5 infantry sub-positions within the tile. Set when an infantry unit occupies that slot.
  TerrainTileFlags1_InfantrySlot1 = 0x2,
  TerrainTileFlags1_InfantrySlot2 = 0x4,
  TerrainTileFlags1_InfantrySlot3 = 0x8,
  TerrainTileFlags1_InfantrySlot4 = 0x10,
  TerrainTileFlags1_Obstructed = 0x20,  ///< Obstructed terrain (edges of landscape features)
  TerrainTileFlags1_Blocked = 0x40,     ///< Blocked terrain (impassable - buildings with speed=0 also set this)
  TerrainTileFlags1_Impassible = 0x60,  ///< Obstructed|Blocked
  TerrainTileFlags1_VehicleOrBuilding = 0x80, ///< Vehicle/building occupying the tile (a single entity fills the whole tile)
};

/* 224 */
enum KKND::TerrainTileFlags2 : unsigned __int8
{
  TerrainTileFlags2_None = 0x0,
  TerrainTileFlags2_MoverBlocksSlot0 = 0x1, ///< The XOR pattern (flags ^ flags2) & 0x1F is the key trick — it detects tiles where occupied slots, returning what tile slots can be attempted to path through
  TerrainTileFlags2_MoverBlocksSlot1 = 0x2,
  TerrainTileFlags2_MoverBlocksSlot2 = 0x4,
  TerrainTileFlags2_MoverBlocksSlot3 = 0x8,
  TerrainTileFlags2_MoverBlocksSlot4 = 0x10,
  TerrainTileFlags2_CantPlaceBuilding = 0x40,
  TerrainTileFlags2_OilPatch = 0x80,
};

/* 222 */
struct KKND::TerrainTile
{
  KKND::TerrainTileFlags1 flags1;
  KKND::TerrainTileFlags2 flags2;
  char _pad_2;
  char _pad_3;
  KKND::Unit *units[5];
};

/* 230 */
struct KKND::TechLevels
{
  int num_buildings_by_level[5];
  int _tech_levels_unused_field_10;
  int _tech_levels_unused_field_14;
  int _tech_levels_unused_field_18;
  int _tech_levels_unused_field_1C;
  int max_level;                        ///< memset sizes are inconsistent throughout the codebase. should be of size 28   BUG fix 24 size memsets
};

/* 248 */
enum KKND::BoxdPathingClassification : unsigned __int32
{
  Boxd_Impassable = 0,                  ///< Terrain wall — impassable ground, no building on it
  Boxd_PartiallyOccupied = 1,           ///< Partially occupied — entities present but slots in flux (room to squeeze)
  Boxd_Clear = 2,                       ///< Free/clear tile — walkable, nobody there (the unit itself is not counted as obstacle)
  Boxd_FullyOccupied = 3,               ///< Fully occupied — all entity slots stably filled / building
  Boxd_MovementSucceeded = 4,           ///< Function is really a dual-purpose "try-move-or-classify" — returns classification only when move fails, returns 4 when move succeeds. This makes it a tagged union of success(4) vs failure-reason(0-3)
};

/* 251 */
struct KKND::TankerState
{
  int oil_loaded;
  KKND::Unit *current_destination;
  KKND::Unit *powerplant;
  KKND::Unit *drillrig;
  int drillrig_unit_id;
  int powerplant_unit_id;
  int current_destination_unit_id;
  int num_destinations;
  KKND::Unit *destinations[20];
  KKND::MobdImageData *img;
};

/* 254 */
enum KKND::GameEventType : unsigned __int8
{
  GameEvent_None = 0,
  GameEvent_SelectedBox = 1,
  GameEvent_SelectedUnit = 2,
  GameEvent_SelectionCleared = 3,
  GameEvent_AssignedControlGroup = 4,
  GameEvent_RecalledControlGroup = 5,
  GameEvent_MoveCommand = 6,
  GameEvent_AttackCommand = 7,
  GameEvent_UnitProduced = 8,
  GameEvent_BuildingPlaced = 9,
  GameEvent_MobileBaseDeployed = 10,
  GameEvent_BuildingConstructionProgressed = 11,
  GameEvent_UpgradeCompleted = 12,
  GameEvent_RepairBayHealTick = 13,
  GameEvent_14 = 14,
  GameEvent_GamePaused = 15,
  GameEvent_GameResumed = 16,
  GameEvent_NetzPlayerDisconnected = 17,
  GameEvent_18 = 18,
  GameEvent_TankerAssignedToDrillrig = 19,
  GameEvent_TankerAssignedToPowerPlant = 20,
  GameEvent_RepairBayAssigned = 21,
  GameEvent_TechnicianAssigned = 22,
  GameEvent_23 = 23,
  GameEvent_InfiltratorAssigned = 24,
  GameEvent_AirstrikeCalled = 25,
  GameEvent_BuildingSold = 26,
  GameEvent_SwearAllegiance = 27,
};

/* 255 */
/// Game Events are primitives that should be synched during the multiplayer game
/// (during the local game they're just dequeued locally).
/// One event is dequeued per tick
/// 
/// Verified NOT KKND::NetzGameEvent
struct KKND::GameEvent
{
  KKND::GameEventType type;
  char payload[12];
  bool ready_to_consume;                ///< when produced, is set to true
                                        ///< when NETZ consumes the event, is set to false
};

/* 256 */
struct __unaligned GameEventNode
{
  GameEventNode *next;
  KKND::GameEvent evt;
  char field_12;
  char field_13;
};

/* 259 */
enum KKND::UnitCommandArchetype : unsigned __int32
{
  UnitCommandArchetype_None = 0,
  UnitCommandArchetype_Tanker = 1,
  UnitCommandArchetype_Lab = 2,
  UnitCommandArchetype_Infiltrator = 3,
  UnitCommandArchetype_Technician = 4,
  UnitCommandArchetype_5 = 5,
  UnitCommandArchetype_CombatVehicle = 6,
  UnitCommandArchetype_MobileBase = 7,
  UnitCommandArchetype_MobileDerrick = 8,
  UnitCommandArchetype_CombatInfantry = 9,
};

/* 257 */
struct KKND::CursorState
{
  KKND::CursorUnitSelection *selection_head;
  KKND::CursorUnitSelection *selection_tail;
  KKND::Task *_cursor_state_task_8;
  KKND::CursorUnitSelection *selection_pool;
  KKND::CursorUnitSelection *selection_free_head;
  KKND::Task *cursor_task;
  KKND::Task *hovered_ui_task;
  KKND::Task *hovered_unit_task;
  int cursor_mobd_offset;
  BOOL is_help_mode;
  BOOL is_airstrike_targeting;
  BOOL is_placing_building;
  BOOL is_selling_building;
  BOOL is_cursor_over_impassable_terrain;
  BOOL are_own_units_selected;
  BOOL is_single_unit_selected;
  BOOL are_movable_units_selected;
  BOOL are_attacker_units_selected;
  KKND::UnitType unit_type_to_voice_response;
  BOOL is_rmb_scrolling;                ///< RMB drag = scroll camera.
                                        ///< On release, if total drift was tiny (< 4096 in both axes = less than ~16px), treat it as right-click → deselect all.
                                        ///< Otherwise it was intentional camera pan, no deselect.
  int rmb_scrolling_initial_x;
  int rmb_scrolling_initial_y;
  int rmb_scrolling_max_dx;
  int rmb_scrolling_max_dy;
  int rmb_scrolling_dx;
  int rmb_scrolling_dy;
  KKND::Unit *selection_executing_representative; ///< When a mixed group of units selected, determine which unit takes command priority
                                                  ///< I.e: a lab and a technic is slected at the same time. Clicking on the lab would start reseach rather than repairs because lab takes execution priority
  KKND::UnitCommandArchetype selection_executing_archetype;
  KKND::Entity *cursor_hitbox_tester;
  KKND::Entity *cursor_entity;
  KKND::BuildingPlannerPayload *planner;
};

/* 258 */
struct KKND::CursorUnitSelection
{
  KKND::CursorUnitSelection *next;
  KKND::CursorUnitSelection *prev;
  KKND::Task *unit_task;
};

/* 260 */
struct KKND::BuildingPlannerPayload
{
  KKND::UnitType type;
  int footprint_width;
  int footprint_height;
  int cost;
};

/* 261 */
struct KKND::BuildingBlueprint
{
  KKND::UnitType type;
  int footprint_width;
  int footprint_height;
  int _building_blueprint_field_C_unused; ///< Values: 25–1000. Never accessed in the code — cost is taken from unit stats table instead. Vestigial data, possibly original dev cost or HP table.
  int _building_blueprint_field_10_unused; ///< Values: 5 (towers), 10–15 (utility), 25 (production). Pattern = construction duration. Also never accessed in the code.
  unsigned __int32 collision_mask;       ///< So bit=1 means solid tile, bit=0 means passable tile in building footprint.
                                         ///< 
                                         ///< Examples:
                                         ///<     Guard Tower (2×2): 0xC0000000 = 11 00 → top row solid, bottom row passable (entrance)
                                         ///<     Drill Rig (3×2): 0xE0000000 = 111 000 → top row solid, bottom passable
                                         ///<     Machine Shop (4×4): 0xFF000000 = 1111 1111 0000 0000 → top 2 rows solid, bottom 2 passable
                                         ///<     Walls (1×1): 0xFFFFFFFF → all solid (every bit set)
};

/* 263 */
enum KKND::ConstructStage : unsigned __int32
{
  BuildingConstructionStage_1 = 1,
  BuildingConstructionStage_2 = 2,
  BuildingConstructionStage_Complete = 3,
};

/* 262 */
struct KKND::Construct
{
  KKND::Construct *next;
  KKND::Construct *prev;
  int unit_id;
  int player_num;
  KKND::ConstructStage stage;
  int cost;
  int remaining_cost;
  int cost_per_tick;                    ///< (cost << 8) / (60 * build_time) -- 60 is the global FPS constant (or rather ticks per second)
};

/* 267 */
enum KKND::MouseActions : unsigned __int32
{
  Mouse_MoveUp = 0x1,
  Mouse_MoveDown = 0x2,
  Mouse_MoveLeft = 0x4,
  Mouse_MoveRight = 0x8,
  Mouse_LButton = 0x10,
  Mouse_RButton = 0x20,
  Mouse_MButton = 0x80,
};

/* 264 */
struct KKND::MouseState
{
  KKND::MouseActions held_actions_mask;
  KKND::MouseActions new_actions_mask;
  KKND::MouseActions released_actions_mask;
  KKND::Direction direction;            ///< mouse move direction (not used)
  fixed cursor_x;
  fixed cursor_y;
  fixed cursor_dx;
  fixed cursor_dy;
};

/* 268 */
enum KKND::KeyboardActions : unsigned __int32
{
  Keyboard_Up = 0x1,
  Keyboard_Down = 0x2,
  Keyboard_Left = 0x4,
  Keyboard_Right = 0x8,
  Keyboard_Arrows = 0xF,
  Keyboard_Ctrl = 0x10,
  Keyboard_Numeric = 0x20,
  Keyboard_Shift = 0x40,
  Keyboard_Enter = 0x80,
  Keyboard_Tab = 0x100,
  Keyboard_Esc = 0x200,
};

/* 270 */
enum KKND::KeyboardDosScancodes : unsigned __int32
{
  Scancode_None = 0,
  Scancode_Ctrl = 29,
  Scancode_Alt = 56,
  Scancode_Q = 16,
  Scancode_W = 17,
  Scancode_T = 20,
  Scancode_A = 30,
  Scancode_M = 50,
  Scancode_1 = 2,
  Scancode_2 = 3,
  Scancode_3 = 4,
  Scancode_4 = 5,
  Scancode_5 = 6,
  Scancode_6 = 7,
  Scancode_7 = 8,
  Scancode_8 = 9,
  Scancode_9 = 10,
  Scancode_0 = 11,
};

/* 265 */
struct KKND::KeyboardState
{
  KKND::KeyboardActions held_actions_mask;
  KKND::KeyboardActions new_actions_mask;
  KKND::KeyboardActions released_actions_mask;
  KKND::Direction direction;            ///< Current *arrow* key combo direction e.g top and right keys held => Direction_NE
  KKND::KeyboardDosScancodes last_key_scancode; ///< 2-11: CTRL+num
                                                ///< 30 -> 'A'
                                                ///< ...
};

/* 271 */
struct KKND::SidebarButton
{
  KKND::SidebarButton *next;
  KKND::SidebarButton *prev;
  KKND::Task *task;
  void (__fastcall *mode_open)(KKND::SidebarButton *);
  void (__fastcall *mode_close)(KKND::SidebarButton *);
  ptrdiff_t icon_mobd_frame;
  int base_cost;                        ///< for progress bar calc
  KKND::ProductionSharedState *production_state;
  void *ctx;                            ///< - RenderString* for cash button
                                        ///< - SidebarFactoryProductionOption* for unit/vehicle buttons
                                        ///< - UnitType for building buttons
  KKND::Entity *entity;
};

/* 281 */
struct KKND::ProductionSharedState
{
  int remaining_cost;
  int num_orders;
  KKND::Entity *progress_bar;
  KKND::Entity *queue_size_label;
};

/* 272 */
/// One for each type of parent factory's production (tank, tanker, etc) - see FactoryProduction
/// Player can enqueue more than one of the same unit, but it's managed in the sidebar logic
struct KKND::FactoryProdJob
{
  KKND::FactoryProdJob *next;
  KKND::FactoryProdJob *prev;
  int base_bandwidth;                   ///< max amount of money factory can spend per tick
  int effective_bandwidth;              ///< effective amount of money this production receives based on number of other concurrent productions
  int base_cost;                        ///< base total cost of the unit
  int *remaining_cost;                  ///< ptr to the remaining cost (managed by sidebar); reset back to base_cost if multiple units in the queue (sidebar)
  int *remaining_cash;                  ///< ptr to player's cash (managed separately)
  int accumulator;                      ///< each tick: += effective_bandwith;  256 bandwidth spent = $1 towards production
  int *num_orders;                      ///< num of the same unit ordered in sidebar; when production finishes, if num_orders > 1 - remaining_cost resets and production continues (actual value stored and managed in sidebar)
  KKND::SidebarFactoryProductionOption *notification_arg; ///< Param to send with Unit Ready msg. For sidebar units = ProductionOption *. For AI/buildings -> NULL
  KKND::Task *notification_task;        ///< Task that receives Unit Ready msg when cost hits 0. For sidebar = g_game_update_loop_task. For AI = entity script or 0
};

/* 275 */
/// One per buildable unit type within SidebarFactoryProduction parent
struct KKND::SidebarFactoryProductionOption
{
  KKND::SidebarFactoryProductionOption *next;
  KKND::SidebarFactoryProductionOption *prev;
  KKND::Unit *factory;                  ///< factory unit ptr
  KKND::UnitType product_type;          ///< type of unit being produced
  ptrdiff_t icon_mobd_frame;            ///< sidebar icon mobd offset
  KKND::ProductionSharedState state;    ///< passed to factory production as a pointer
  int base_cost;
  int production_time;                  ///< unit_stats.production_time x 60; bandwidth = (cost << 8) / production_time_x60
  int key;                              ///< Grouping key : slot_index + 16 * PRODUCTION_GROUP_ID. Used to match new jobs to existing factory. -1 = ungrouped (AI/building construction)14.
};

/* 276 */
enum KKND::SidebarFactoryProductionType : unsigned __int32
{
  ProductionType_Infantry = 0,
  ProductionType_Vehicles = 1,
  ProductionType_Buildings = 2,
  ProductionType_Towers = 3,
  ProductionType_Aircraft = 4,
  ProductionType_Blacksmith = 5,
};

/* 273 */
struct KKND::SidebarFactoryProduction
{
  KKND::SidebarFactoryProduction *next;
  KKND::SidebarFactoryProduction *prev;
  KKND::SidebarFactoryProductionType type;
  KKND::Unit *factory_or_factory_type;
  KKND::Sidebar *sidebar;
  KKND::SidebarFactoryProductionOption *prod_head;
  KKND::SidebarFactoryProductionOption *prod_tail;
  KKND::Unit *base_building_for_aircraft_spawn;
  int _sidebar_factory_production_field_20;
  int _sidebar_factory_production_field_24;
  int _sidebar_factory_production_field_28;
  int _sidebar_factory_production_field_2C;
  int _sidebar_factory_production_field_30;
  int _sidebar_factory_production_field_34;
  int _sidebar_factory_production_field_38;
  int _sidebar_factory_production_field_3C;
  int key;
  int factory_header_color_idx;         ///< multiple factories of the same type have differently coloured header bars
  KKND::Entity *icon_entity;
};

/* 277 */
struct KKND::Sidebar
{
  KKND::Sidebar *next;
  KKND::Sidebar *prev;
  KKND::Task *task;
  int num_buttons;
  fixed x;
  fixed y;
  int h_spacing;
  int v_spacing;
  KKND::Entity *entity;
  KKND::SidebarButton *button_list_head;
  KKND::SidebarButton *button_list_tail;
  int _sidebar_field_2C_unused;
  int _sidebar_field_30_unused;
  int _sidebar_field_34_unused;
  int _sidebar_field_38_unused;
  int _sidebar_field_3C_unused;
  int _sidebar_field_40_unused;
  int _sidebar_field_44_unused;
  int _sidebar_field_48_unused;
};

/* 274 */
/// Production per individual factory
struct KKND::FactoryProd
{
  KKND::FactoryProd *next;
  KKND::FactoryProd *prev;
  KKND::FactoryProdJob *jobs_head;
  KKND::FactoryProdJob *jobs_tail;
  int _factory_production_field_10_unused;
  int _factory_production_field_14_unused;
  int _factory_production_field_18_unused;
  int _factory_production_field_1C_unused;
  int _factory_production_field_20_unused;
  int _factory_production_field_24_unused;
  int _factory_production_field_28_unused;
  int _factory_production_field_2C_unused;
  int _factory_production_field_30_unused;
  int _factory_production_field_34_unused;
  int _factory_production_field_38_unused;
  int key;                              ///< Grouping key : slot_index + 16 * PRODUCTION_GROUP_ID. Used to match new jobs to existing factory. -1 = ungrouped (AI/building construction)
  int num_active_productions;           ///< the length of the production list (production nodes get recycled on production complete)
};

/* 278 */
struct KKND::BuildingState
{
  void *ctx;                            ///< drillrig: OilPatch*
                                        ///< lab: Task* for the research task
  int upgrade_level;
  int upgrade_timer;
  __int16 same_building_count;
  __int16 garrison_strength;            ///< Init from params (typically 5). Friendly unit enters → increment (max 5). Enemy saboteur → decrement. Zero = building captured/destroyed.
  KKND::SidebarFactoryProduction *prod;
  KKND::MobdImageData *status_bar;
  KKND::Unit *docked_tanker;
  int docked_tanker_unit_id;
  KKND::Entity *repair_anim;            ///< wrench
  int num_active_repairs;               ///< e.g 2 technicians enter the building
};

/* 284 */
struct KKND::Bomber
{
  KKND::Bomber *next;
  KKND::Bomber *prev;
  KKND::Unit *unit;
};

/* 286 */
struct KKND::TankerConvoyState
{
  int x;
  int y;
  int checkpoint;
};

/* 287 */
struct KKND::Vec2
{
  int x;
  int y;
};

/* 288 */
struct KKND::BuildLimits
{
  KKND::BuildLimits *next;
  KKND::BuildLimits *prev;
  KKND::UnitType type;
  int num_buildings_of_this_type;
};

/* 289 */
struct KKND::BuildingStartingProduction
{
  unsigned __int32 mask;
  KKND::UnitType type;
  ptrdiff_t mobd_lookup;
};

/* 290 */
struct KKND::DrillrigState
{
};

/* 291 */
struct KKND::Scar
{
  KKND::Scar *next;
  KKND::Scar *prev;
  ptrdiff_t mobd_frame;
  KKND::Entity *entity;
};

/* 292 */
struct KKND::Nuke
{
  void (__fastcall *mode)(KKND::Nuke *);
  KKND::Entity *entity;
  KKND::Task *task;
};

/* 293 */
struct KKND::UpgradeProcess
{
  void (__fastcall *mode)(KKND::UpgradeProcess *);
  int pulse_cooldown;
  int stage;                            ///< 1 to 6
  BOOL cancelled;
  KKND::Unit *building;
  int _upgrade_state_field_14;
  KKND::Entity *progress_bar;
  KKND::Task *task;
};

/* 294 */
struct KKND::ScriptType4
{
  KKND::MobdId mobd_id;
  void (__cdecl __noreturn *handler)(KKND::Task *);
  KKND::TaskKind kind;
  int script_type_4_field_C;
  int script_type_4_field_10;
  KKND::UnitType unit_type;
};

/* 295 */
/// might be type 4 with extra padding
struct KKND::ScriptType1
{
  KKND::MobdId mobd_id;
  void (__cdecl *handler)(KKND::Task *);
  KKND::TaskKind kind;
  int script_type_1_field_C;
  int script_type_1_field_10;
  KKND::UnitType unit_type;
  int script_type_1_field_18;
  int script_type_1_field_1C;
  int script_type_1_field_20;
  int script_type_1_field_24;
};

/* 296 */
/// might be type 4 with extra padding
struct KKND::ScriptType2
{
  KKND::MobdId mobd_id;
  void (__cdecl *handler)(KKND::Task *);
  KKND::TaskKind kind;
  int script_type_2_field_C;
  int script_type_2_field_10;
  KKND::UnitType unit_type;
  int script_type_2_field_18;
  int script_type_2_field_1C;
};

/* 297 */
struct KKND::ScriptType3
{
  KKND::MobdId mobd_id;
  void (__cdecl *handler)(KKND::Task *);
  KKND::TaskKind kind;
  int script_type_3_field_C;
  int script_type_3_field_10;
  int script_type_3_field_14;
  int script_type_3_field_18;
  int script_type_3_field_1C;
  KKND::UnitType unit_type;
  int script_type_3_field_24;
  int script_type_3_field_28;
  int script_type_3_field_2C;
};

/* 298 */
enum KKND::ScriptType
{
  ScriptType_DetentionCenter = 3,
  ScriptType_HoldingPens = 4,
  ScriptType_Clanhall = 5,
  ScriptType_Outpost = 6,
};

/* 299 */
struct KKND::UiStr
{
  KKND::UiStr *next;
  KKND::UiStr *prev;
  KKND::Glyph *glyphs;
  int cols;
  int rows;
  KKND::FontMobd *font;
  int cursor_col;
  int cursor_row;
};

/* 300 */
struct KKND::Glyph
{
  KKND::Glyph *next;
  KKND::RenderNode *rn;
};

/* 428 */
struct KKND::FontMobd
{
  int _font_mobd_0;
  KKND::GlyphDesc *glyphs[];
};

/* 427 */
struct KKND::GlyphDesc
{
  int x;
  int y;
  int _glyph_desc_field_8;
  KKND::RenderImage *img;
  void *_glyph_desc_field_10;
  void *_glyph_desc_field_14;
  void *advance;
};

/* 304 */
/// formerly _stru9_unit_order_2
struct KKND::SelectionNode
{
  KKND::SelectionNode *next;
  KKND::SelectionNode *prev;
  KKND::Task *unit_task;
};

/* 302 */
struct KKND::AttackOrderPayload
{
  int player_num;
  KKND::Unit *target;
};

/* 301 */
struct KKND::UnitOrderCtx
{
  KKND::SelectionNode sentinel;
  KKND::SelectionNode *selection_pool;
  KKND::SelectionNode *selection_head;
  KKND::Task *_stru9_unit_task_field_14;
  int player_num;
  int owns_selection;
  KKND::AttackOrderPayload attack_order;
  KKND::AttackOrderPayload move_order;
  int _stru9_unit_task_field_30;
};

/* 303 */
struct KKND::MoveOrderPayload
{
  int player_num;
  int dst_x;
  int dst_y;
};

/* 305 */
struct KKND::AiController
{
  KKND::AiController *next;
  KKND::AiController *prev;
  void *ctx1;
  void *ctx2;
  KKND::AiUnitNode *unit_node_pool;
  KKND::AiUnitNode *unit_free_head;
  KKND::AiWandererNode *new_wanderer_head;
  KKND::AiWandererNode *new_wanderer_tail;
  int _ai_controller_20_ai_strategic_value_threshold;
  int _ai_controller_24;
  KKND::AiWandererNode *wanderer_pool;
  KKND::AiWandererNode *wanderer_free_head;
  KKND::AiWandererNode *active_wanderer_head;
  KKND::AiWandererNode *active_wanderer_tail;
  char _ai_controller_38[8];
  KKND::AiWandererNode *active_wanderer_pool;
  KKND::AiWandererNode *active_wanderer_free_head;
  KKND::AiAttackerNode *unassigned_attacker_head;
  KKND::AiAttackerNode *unassigned_attacker_tail;
  char _ai_controller_50[8];
  KKND::AiAttackerNode *attacker_pool;
  KKND::AiAttackerNode *attacker_free_head;
  KKND::AiAttackerNode *convoy_escort_head;
  KKND::AiAttackerNode *convoy_escort_tail;
  char _ai_controller_68[8];
  KKND::AiAttackerNode *convoy_escort_pool;
  KKND::AiAttackerNode *convoy_escort_free_head;
  KKND::AiBuildNode *build_head;
  KKND::AiBuildNode *build_tail;
  char _ai_controller_80[20];
  KKND::AiBuildNode *build_pool;
  KKND::AiBuildNode *build_free_head;
  KKND::AiDrillrigNode *drillrig_head;
  KKND::AiDrillrigNode *drillrig_tail;
  char _ai_controller_A4[44];
  int _ai_controller_D0;
  KKND::AiDrillrigNode *drillrig_pool;
  KKND::AiDrillrigNode *drillrig_free_head;
  KKND::AiTankerNode *tanker_head;
  KKND::AiTankerNode *tanker_tail;
  int _ai_controller_E4;
  int _ai_controller_E8;
  KKND::AiTankerNode *tanker_pool;
  KKND::AiTankerNode *tanker_free_head;
  KKND::AiPowerPlantNode *powerplant_head;
  KKND::AiPowerPlantNode *powerplant_tail;
  int _ai_controller_FC;
  KKND::AiPowerPlantNode *powerplant_pool;
  KKND::AiPowerPlantNode *powerplant_free_head;
  KKND::AiEnemyNode *enemy_head;
  KKND::AiEnemyNode *enemy_tail;
  int _ai_controller_110;
  KKND::AiEnemyNode *enemy_pool;
  KKND::AiEnemyNode *enemy_free_head;
  KKND::AiSquadNode *attack_squad_head;
  KKND::AiSquadNode *attack_squad_tail;
  char _ai_controller_124[60];
  KKND::AiSquadNode *squad_pool;
  KKND::AiSquadNode *squad_pool_free_head;
  KKND::AiSquadNode *patrol_squad_head;
  KKND::AiSquadNode *patrol_squad_tail;
  char _ai_controller_170[60];
  int _ai_controller_1AC;
  int _ai_controller_1B0;
  KKND::AiSquadNode *retreat_squad_head;
  KKND::AiSquadNode *retreat_squad_tail;
  char _ai_controller_1BC[60];
  int _ai_controller_1F8;
  int _ai_controller_1FC;
  int _ai_controller_200_head;
  int _ai_controller_200_tail;
  char _ai_controller_208[60];
  int _ai_controller_244;
  int _ai_controller_248;
  KKND::AiSquadNode *staging_squad;
  int base_area_min_x;
  int base_area_min_y;
  int base_area_max_x;
  int base_area_max_y;
  KKND::AiBuildOrderNode *build_order_head;
  KKND::AiBuildOrderNode *build_order_tail;
  int _ai_controller_268;
  KKND::AiBuildOrderNode *build_order_pool;
  KKND::AiBuildOrderNode *build_order_free_head;
  KKND::AiBuildOrderNode *build_order_current;
  int rally_x;
  int rally_y;
  KKND::Vec2 patrol_waypoints[4];
  int player_num;
  KKND::Race player_race;
  int *cash;
  int attacker_count;
  int max_units;
  int squad_threshold;
  int attack_confidence;
  int base_threat;
  int max_squad_threat;
  int best_patrol_waypoint_idx;
  int patrol_threat[5];
  KKND::UnitType last_unit_produced;
  KKND::UnitType last_unit_produced_factory;
  BOOL tanker_production_in_progress;
  KKND::AiDrillrigNode *preferred_drillrig;
  KKND::AiBuildingPlacementNode *building_replacement_head;
  KKND::AiBuildingPlacementNode *building_replacement_tail;
  char _ai_controller_2F4[28];
  KKND::AiBuildingPlacementNode *building_replacement_pool;
  KKND::AiBuildingPlacementNode *building_replacement_free_head;
  KKND::AiBuildingPlacementNode *drillrig_replacement_head;
  KKND::AiBuildingPlacementNode *drillrig_replacement_tail;
  char _ai_controller_320[28];
  int _ai_controller_33C;
  int _ai_controller_340;
  int construction_state;
  int construction_base_cost;
  int construction_remaining_cost;
  int construction_countdown;           ///< small 10 ticks stagger before placing a new building
  int construction_cost_per_tick;
  KKND::Task *construction_task;        ///< current building under construction
  int airstrike_interval;
  int airstrike_count;
};

/* 307 */
struct KKND::AiUnitNode
{
  KKND::AiUnitNode *next;
  KKND::AiUnitNode *prev;
  KKND::Unit *unit;
  KKND::AiSquadNode *squad;
};

/* 308 */
struct KKND::AiWandererNode
{
  KKND::AiWandererNode *next;
  KKND::AiWandererNode *prev;
  int _ai_wanderer_node_8;
  KKND::Unit *unit;
};

/* 310 */
struct KKND::AiAttackerNode
{
  KKND::AiAttackerNode *next;
  KKND::AiAttackerNode *prev;
  KKND::AiSquadNode *squad;
  KKND::Unit *unit;
};

/* 311 */
struct KKND::AiBuildNode
{
  KKND::AiBuildNode *next;
  KKND::AiBuildNode *prev;
  KKND::Unit *unit;
  int remaining_cost;
  KKND::UnitType unit_type;
  int base_cost;
  int cost_per_tick;
};

/* 312 */
struct KKND::AiDrillrigNode
{
  KKND::AiDrillrigNode *next;
  KKND::AiDrillrigNode *prev;
  KKND::AiTankerNode *tanker_next;
  KKND::AiTankerNode *tanker_prev;
  int _ai_drillrig_node_10;
  int _ai_drillrig_node_14;
  int _ai_drillrig_node_18;
  int _ai_drillrig_node_1C;
  KKND::Unit *unit;
  KKND::AiSquadNode *guard_squad;
  KKND::AiPowerPlantNode *nearest_powerplant;
  int local_threat;
  int current_tanker_count;
  int desired_tanker_count;
};

/* 313 */
struct KKND::AiTankerNode
{
  KKND::AiTankerNode *next;
  KKND::AiTankerNode *prev;
  KKND::AiDrillrigNode *drillrig;
  KKND::Unit *unit;
};

/* 314 */
struct KKND::AiPowerPlantNode
{
  KKND::AiPowerPlantNode *next;
  KKND::AiPowerPlantNode *prev;
  KKND::Unit *unit;
};

/* 315 */
struct KKND::AiEnemyNode
{
  KKND::AiEnemyNode *next;
  KKND::AiEnemyNode *prev;
  KKND::Unit *unit;
};

/* 316 */
struct KKND::AiSquadNode
{
  KKND::AiSquadNode *next;
  KKND::AiSquadNode *prev;
  KKND::AiSquadNode *merge_target_squad;
  KKND::AiAttackerNode *attackers_head;
  KKND::AiAttackerNode *attackers_tail;
  int _ai_stru160_node_14_unused;
  int _ai_stru160_node_18_unused;
  int flags;
  KKND::AiAttackerNode *last_attacker;
  KKND::AiEnemyNode *enemy_target;
  int retarget_cooldown;
  int total_squad_threat;
  int area_threat;
  int center_x;
  int center_y;
  int destination_x;
  int destination_y;
};

/* 317 */
struct KKND::AiBuildOrderNode
{
  KKND::AiBuildOrderNode *next;
  KKND::AiBuildOrderNode *prev;
  int _ai_stru26C_node_8;
};

/* 318 */
struct KKND::AiBuildingPlacementNode
{
  KKND::AiBuildingPlacementNode *next;
  KKND::AiBuildingPlacementNode *prev;
  KKND::Unit *unit;
  KKND::UnitType unit_type;
  int unit_x;
  int unit_y;
  int grid_anchor_x;
  int grid_anchor_y;
  int strategic_value;
};

/* 306 */
struct KKND:___void_ptr_in_fact_:AiController_struC
{
  KKND::AiSquadNode *_ai_controller_strucC_0;
  int _ai_controller_strucC_4;
  int _ai_controller_strucC_8;
  KKND::AiAttackerNode *_ai_controller_strucC_C;
  int _ai_controller_strucC_10;
  int _ai_controller_strucC_14;
  int _ai_controller_strucC_18;
  int _ai_controller_strucC_1C;
  int _ai_controller_strucC_20;
  int _ai_controller_strucC_24;
  int _ai_controller_strucC_28;
  int _ai_controller_strucC_2C;
  int _ai_controller_strucC_30;
  int _ai_controller_strucC_34;
  int _ai_controller_strucC_38;
  int _ai_controller_strucC_3C;
  int _ai_controller_strucC_40;
  int _ai_controller_strucC_44;
  int _ai_controller_strucC_48;
  int _ai_controller_strucC_4C;
  int _ai_controller_strucC_50;
  int _ai_controller_strucC_54;
  int _ai_controller_strucC_58;
  int _ai_controller_strucC_5C;
  int _ai_controller_strucC_60;
  int _ai_controller_strucC_64;
  int _ai_controller_strucC_68;
  int _ai_controller_strucC_6C;
  int _ai_controller_strucC_70;
  int _ai_controller_strucC_74;
  int _ai_controller_strucC_78;
  int _ai_controller_strucC_7C;
  int _ai_controller_strucC_80;
  int _ai_controller_strucC_84;
  int _ai_controller_strucC_88;
  int _ai_controller_strucC_8C;
  int _ai_controller_strucC_90;
  int _ai_controller_strucC_94;
  int _ai_controller_strucC_98;
  int _ai_controller_strucC_9C;
  int _ai_controller_strucC_A0;
  int _ai_controller_strucC_A4;
  int _ai_controller_strucC_A8;
  int _ai_controller_strucC_AC;
  int _ai_controller_strucC_B0;
  int _ai_controller_strucC_B4;
  int _ai_controller_strucC_B8;
  int _ai_controller_strucC_BC;
  int _ai_controller_strucC_C0;
  int _ai_controller_strucC_C4;
  int _ai_controller_strucC_C8;
  int _ai_controller_strucC_CC;
  int _ai_controller_strucC_D0;
  int _ai_controller_strucC_D4;
  int _ai_controller_strucC_D8;
  int _ai_controller_strucC_DC;
  int _ai_controller_strucC_E0;
  int _ai_controller_strucC_E4;
  int _ai_controller_strucC_E8;
  int _ai_controller_strucC_EC;
  int _ai_controller_strucC_F0;
  int _ai_controller_strucC_F4;
  int _ai_controller_strucC_F8;
  int _ai_controller_strucC_FC;
  int _ai_controller_strucC_100;
  int _ai_controller_strucC_104;
  int _ai_controller_strucC_108;
  int _ai_controller_strucC_10C;
  int _ai_controller_strucC_110;
  int _ai_controller_strucC_114;
  int _ai_controller_strucC_118;
  int _ai_controller_strucC_11C;
  int _ai_controller_strucC_120;
  int _ai_controller_strucC_124;
  int _ai_controller_strucC_128;
  int _ai_controller_strucC_12C;
  int _ai_controller_strucC_130;
};

/* 309 */
/// AiWandererNode
struct KKND::____delete___Ai_stru40_Node
{
  KKND::AiWandererNode *next;
  KKND::AiWandererNode *prev;
  int _ai_wanderer_node_8;
  KKND::Unit *unit;
};

/* 389 */
struct KKND::AiSquadNodeSaveStruct
{
  __int32 num_attacker_nodes;
  __int32 enemy_target_unit_id;
  __int32 attacker_head_unit_id;
  __int32 retarget_cooldown;
  __int32 total_squad_threat;
  __int32 area_threat;
  __int32 center_x;
  __int32 center_y;
  __int32 destination_x;
  __int32 destination_y;
};

/* 319 */
const struct KKND::AiPlayersSaveStruct
{
  int ai_task_handler_id;
  int num_unit_nodes;
  int num_wanderer_nodes;
  int _ai_players_save_struct_C;
  int _ai_players_save_struct_10;
  int _ai_players_save_struct_14;
  int _ai_players_save_struct_18;
  int _ai_players_save_struct_1C;
  int _ai_players_save_struct_20;
  int _ai_players_save_struct_24;
  int _ai_players_save_struct_28;
  int _ai_players_save_struct_2C;
  int _ai_players_save_struct_30;
  int _ai_players_save_struct_34;
  int _ai_players_save_struct_38;
  int _ai_players_save_struct_3C;
  int _ai_players_save_struct_40;
  int _ai_players_save_struct_44;
  int _ai_players_save_struct_48;
  int _ai_players_save_struct_4C;
  int _ai_players_save_struct_50;
  int _ai_players_save_struct_54;
  int _ai_players_save_struct_58;
  int _ai_players_save_struct_5C;
  int _ai_players_save_struct_60;
  int _ai_players_save_struct_64;
  int _ai_players_save_struct_68;
  int _ai_players_save_struct_6C;
  int _ai_players_save_struct_70;
  int _ai_players_save_struct_74;
  int _ai_players_save_struct_78;
  int _ai_players_save_struct_7C;
  int _ai_players_save_struct_80;
  int _ai_players_save_struct_84;
  int _ai_players_save_struct_88;
  int _ai_players_save_struct_8C;
  int _ai_players_save_struct_90;
  int _ai_players_save_struct_94;
  int _ai_players_save_struct_98;
  int _ai_players_save_struct_9C;
  int player_num;
  int _ai_players_save_struct_A4;
  int _ai_players_save_struct_A8;
  int _ai_players_save_struct_AC;
  int _ai_players_save_struct_B0;
  int _ai_players_save_struct_B4;
  int _ai_players_save_struct_B8;
  int _ai_players_save_struct_BC;
  int _ai_players_save_struct_C0;
  int _ai_players_save_struct_C4;
  int _ai_players_save_struct_C8;
  int _ai_players_save_struct_CC;
  int _ai_players_save_struct_D0;
  int _ai_players_save_struct_D4;
  int last_unit_produced;
  int _ai_players_save_struct_DC;
  int _ai_players_save_struct_E0;
  int _ai_players_save_struct_E4;
  int _ai_players_save_struct_E8;
  int _ai_players_save_struct_EC;
  int _ai_players_save_struct_F0;
  int _ai_players_save_struct_F4;
  int _ai_players_save_struct_F8;
  int _ai_players_save_struct_FC;
  int _ai_players_save_struct_100;
  int _ai_players_save_struct_104;
  int _ai_players_save_struct_108;
  int _ai_players_save_struct_10C;
  int _ai_players_save_struct_110;
  int _ai_players_save_struct_114;
  int _ai_players_save_struct_118;
  int unit_free_head_unit_id;
  KKND::AiSquadNodeSaveStruct squad_node;
  int attacker_free_head_unit_id;
  int _ai_players_save_struct_14C;
  int _ai_players_save_struct_150;
  int _ai_players_save_struct_154;
  int _ai_players_save_struct_158;
  int _ai_players_save_struct_15C;
  int _ai_players_save_struct_160;
  int _ai_players_save_struct_164;
  int _ai_players_save_struct_168;
  int _ai_players_save_struct_16C;
  int _ai_players_save_struct_170;
  int _ai_players_save_struct_174;
  int _ai_players_save_struct_178;
  int _ai_players_save_struct_17C;
  int _ai_players_save_struct_180;
  int _ai_players_save_struct_184;
  int _ai_players_save_struct_188;
  int _ai_players_save_struct_18C;
  int _ai_players_save_struct_190;
  int _ai_players_save_struct_194;
  int _ai_players_save_struct_198;
  int _ai_players_save_struct_19C[2][4];
  int _ai_players_save_struct_1BC;
  int _ai_players_save_struct_1D0;
  int _ai_players_save_struct_1D4;
  int _ai_players_save_struct_1D8;
  int _ai_players_save_struct_1DC;
};

/* 371 */
typedef int KKND::CreatureId;

/* 320 */
struct KKND::CreatureIdToUnitId
{
  KKND::CreatureId creature_id;   ///< actually offset into scripts fn table
                                  ///< task type, but referenced as "Creature ID" in EnemyAI
  KKND::UnitType unit_type;
};

/* 322 */
struct KKND::XMark
{
  void (__fastcall *mode)(KKND::XMark *);
  KKND::Task *task;
  KKND::Entity *entity;
};

/* 323 */
struct KKND::AirstrikeSidebar
{
  void (__fastcall *mode)(KKND::AirstrikeSidebar *);
  int num_airstrikes_available;
  KKND::Entity *counter;
  KKND::Entity *button;
  KKND::Task *task;
};

/* 324 */
struct KKND::EntitySaveStruct
{
  __int32 mobd_id;
  __int32 x;
  __int32 y;
  __int32 z_index;
  __int32 x_speed;
  __int32 y_speed;
  __int32 z_speed;
  __int32 mobd_offset;
  __int32 _54_inside_mobd_ptr4;
  __int32 anim_speed;
};

/* 325 */
struct KKND::UnitSaveStruct
{
  __int32 locked_target_unit_id;
  __int32 task_channel;
  __int32 creature_id;
  __int32 message_handler_id;
  __int32 task_transient_events;
  __int32 task_sleep;
  __int32 task_global_events;
  __int32 task_wait_flags;
  __int32 task_field_2C;
  __int32 type;
  __int32 player_num;
  __int32 turret_task_channel;
  __int32 turret_creature_id;
  __int32 turret_message_handler;
  __int32 turret_task_transient_events;
  __int32 turret_task_sleep;
  __int32 turret_task_global_events;
  __int32 turret_task_wait_flags;
  __int32 turret_task_field_2C;
  KKND::EntitySaveStruct turret;
  __int32 turret_target_unit_id;
  __int32 turret_mode;
  __int32 turret_mobd_lookup_id;
  __int32 turret_reload_timer;
  __int32 turret_volley_remaining;
  __int32 turret_volley_reload_time;
  __int32 turret_field_2C;
  __int32 turret_target_unit_id_2;
  __int32 turret_field_34;
  __int32 unit_mode;
  __int32 unit_mode_idle;
  __int32 unit_mode_arrive;
  __int32 unit_mode_attacked;
  __int32 unit_mode_return;
  __int32 unit_mode_turn_return;
  __int32 unit_message_handler;
  KKND::EntitySaveStruct entity;
  __int32 unit_field_78;
  __int32 orientation;
  __int32 unit_field_80;
  __int32 unit_field_84;
  __int32 target_orientation;
  __int32 hitpoints;
  __int32 experience;
  __int32 veterancy;
  __int32 hp_regen_accumulator;
  __int32 hp_regen_rate;
  __int32 tile_position;
  __int32 map_x;
  __int32 map_y;
  __int32 order_next_waypoint_x;
  __int32 order_next_waypoint_y;
  __int32 order_starting_x;
  __int32 order_starting_y;
  __int32 base_anim_speed;
  __int32 path_next_tile_x;
  __int32 path_next_tile_y;
  __int32 path_next_waypoint_tile_x;
  __int32 path_next_waypoint_tile_y;
  __int32 path_scan_direction;
  __int32 path_scan_orientation;
  __int32 order;
  __int32 order_target_unit_id;
  __int32 opportunity_target_unit_id;
  __int32 order_target_unit_id_2;
  __int32 opportunity_target_unit_id_2;
  __int32 active_target_unit_id;
  __int32 order_target_x;
  __int32 order_target_y;
  __int32 last_attacker_unit_id;
  __int32 entity_field_11C;
  __int32 entity_field_120;
  __int32 path_flags;
  __int32 multi_purpose_field_1;
  __int32 cplc_spawn_param;
  __int32 unit_id;
  __int32 multi_purpose_field_3;
  __int32 path_scan_origin_tile_x;
  __int32 path_scan_origin_tile_y;
  __int32 path_scan_iteration;
  __int32 path_scan_max_iterations;
  __int32 path_scan_best_distance;
  __int32 path_scan_best_tile_x;
  __int32 path_scan_best_tile_y;
  __int32 path_scan_best_iteration;
  __int32 path_scan_cur_distance;
  char ray_exit_map_xs[40];
  char ray_exit_map_ys[40];
  char ray_unit_obstacle_map_xs[40];
  char ray_unit_obstacle_map_ys[40];
  char ray_terrain_obstacle_xs[40];
  char ray_terrain_obstacle_ys[40];
  char entity_array_24C[48];
  __int32 nav_obstacle_unit_id;
  __int32 nav_obstacle_unit_id_2;
  __int32 control_groups[7];
  __int16 idle_fidget_timer;
  __int16 last_stuck_tile_x;
  __int16 last_stuck_tile_y;
  __int16 stuck_timer;
};

/* 326 */
struct KKND::BlitterDesc
{
  int hunk;
  BOOL (*mode_init)();
  KKND::Blitter mode_draw;
  int (__fastcall *mode_cleanup)();
};

/* 327 */
struct KKND::RenderBlitter
{
  KKND::RenderBlitter *next;
  KKND::RenderBlitter *prev;
  int hunk;
  int (__fastcall *mode_init)();
  KKND::Blitter mode_render;
  int (__fastcall *mode_cleanup)();
};

/* 375 */
enum KKND::FileFlags : unsigned __int32
{
  File_Stdio = 0x1,
  File_MemoryMapped = 0x2,
};

/* 328 */
struct KKND::File
{
  KKND::FileFlags flags;
  FILE *fp;
  HANDLE map_handle;
  void *map_base;
  void *map_cursor;
  void *map_end;
  KKND::File *next;
  KKND::File *prev;
};

/* 425 */
enum KKND::SoundPlaybackMode : unsigned __int32
{
  SoundPlayback_Normal = -2,
  SoundPlayback_Streaming = -3,
};

/* 336 */
struct KKND::SoundStream
{
  int id;
  IDirectSoundBuffer *dsb;
  KKND::Task *task;
  KKND::File *file;
  uintptr_t thread;
  KKND::SoundPlaybackMode playback_mode;
  int num_play_attempts_while_paused;
  int volume;
  int vol_transition_remaining_ticks;
  int vol_transition_per_tick;
  int pan;
  int pan_transition_remaining_ticks;
  int pan_transition_per_tick;
  int flags;
  KKND::SoundStream *next;
  KKND::SoundStream *prev;
  int streaming_bytes_remaining;
  char filename[32];
  char _stru7_sound_64[42];
  char _stru7_sound_8E[100];
  char _stru7_sound_F2[82];
  int _stru7_sound_144;
};

/* 376 */
enum KKND::MovieBppFlags : unsigned __int16
{
  MovieBpp_8 = 0x1,
  MovieBpp_16 = 0x2,
};

/* 341 */
struct KKND::MovieHeader
{
  KKND::MovieBppFlags bpp;
  __int16 width;
  __int16 height;
  __int16 _movie_header_6;
  __int16 _movie_header_8;
  __int16 num_frames;
  __int16 current_frame;
  __int16 frame_duration_ms;
  void *curent_frame_pixels;
  int sound_samples_per_sec;
  int sound_flags;
  int sound_bytes_num;
  __int16 *sound_bytes;
  int subtitles_len;
  const char *subtitles;
};

/* 372 */
enum KKND::MovieFlags : unsigned __int16
{
  Movie_HasPosition = 0x1,
  Movie_HasAudio = 0x4,
  Movie_HasVideo = 0x8,
  Movie_HasPalette = 0x10,
  Movie_HasTiming = 0x20,
  Movie_HasSubtitles = 0x40,
};

/* 342 */
struct KKND::MovieFrame
{
  int size;
  KKND::MovieFlags flags;
  __int16 x;                            ///< partial update position
  __int16 y;
  __int16 _movie_frame_A;
  char _movie_frame_C[8];
  KKND::MovieBppFlags bpp;
  __int16 width;
  __int16 height;
  __int16 _movie_frame_1A;
  __int16 _movie_frame_1C;
  __int16 num_frames;
  __int16 sound_flags;
  __unaligned __declspec(align(1)) int sound_sample_rate;
  __int16 _movie_frame_26;
  __int16 _movie_frame_28;
  __int16 _movie_frame_2A;
  __int16 _movie_frame_2C;
  __int16 _movie_frame_2E;
  __int16 _movie_frame_30;
  __int16 _movie_frame_32;
  __int16 _movie_frame_34;
  __int16 _movie_frame_36;
  __int16 _movie_frame_38;
  __int16 _movie_frame_3A;
};

/* 343 */
struct __unaligned __declspec(align(1)) KKND::Movie
{
  KKND::MovieHeader header;
  char _movie_2C[372];
  int _movie_1A0[80];
  char _movie_2E0[36];
  int _movie_304[9];
  char _movie_328[8];
  KKND::MovieFlags frame_flags;
  __int16 active_decode_buffer;
  FILE *file;
  size_t first_frame_offset;            ///< for rewinding
  void *decode_buffers[2];
  int block_offset_lut[256];
  KKND::MovieFrame frame;
  char _movie_780[131016];
  char data[1];
};

/* 344 */
struct KKND::FmvFrameImage
{
  KKND::Blitter mode_render;
  int width;
  int height;
  int _movie_frame_image_C;
  int _movie_frame_image_10;
  BOOL interlaced;
  void *pixels;
  int _movie_frame_image_1C;
  int _movie_frame_image_20;
  int _movie_frame_image_24;
  int _movie_frame_image_28;
  int _movie_frame_image_2C;
};

/* 348 */
enum KKND::HunkFixupType : unsigned __int32
{
  HunkFixup_SimplePointer = 0x0,
  HunkFixup_BatchPointer = 0x40000000,
  HunkFixup_Renderer = 0x80000000,
};

/* 353 */
enum KKND::NetzProtocol : unsigned __int32
{
  NetzProtocol_TCP = 0,
  NetzProtocol_IPX = 1,
  NetzProtocol_Serial = 2,
  NetzProtocol_Count = 3,
  NetzProtocol_Invalid = -1,
};

/* 359 */
struct KKND::DpProvider
{
  BOOL active;
  GUID guid;
  const char *name;
  KKND::DpProvider *next;
};

/* 412 */
enum KKND::NetzMessageType : unsigned __int32
{
  NetzMessageType_0 = 0,
  NetzMessageType_Data = 1,
  NetzMessageType_2 = 2,
  NetzMessageType_Connected = 3,
  NetzMessageType_ConnectionReq = 4,
  NetzMessageType_5 = 5,
  NetzMessageType_Disconnected = 6,
  NetzMessageType_ConnectionLost = 7,
};

/* 360 */
struct KKND::NetzMessage
{
  int _netz_stru2_0;
  int _netz_stru2_4;
  int _netz_stru2_8;
  KKND::NetzMessageType type;
  int _netz_stru2_10;
  int _netz_stru2_14;
  int _netz_stru2_18;
  int _netz_stru2_1C;
  void (__fastcall *handler)(KKND::NetzMessage *);
  BOOL is_locally_generated;
  __int16 pkt;
  __int16 _netz_stru2_2A;
  int player_slot;
  DPID dpid;
  void *pkt_payload;
  int pkt_payload_size;
  int _netz_stru2_3C;
  int _netz_stru2_40;
  int _netz_stru2_44;
  int _netz_stru2_48;
  int _netz_stru2_4C;
  int _netz_stru2_50;
  int _netz_stru2_54;
  int _netz_stru2_58;
};

/* 361 */
enum KKND::NetzError : unsigned __int32
{
  NetzError_Ok = 0x0,
  NetzError_OutOfMemory = 0xFE0000,
  NetzError_LinkInUse = 0xFE0001,
  NetzError_NoFreeLinks = 0xFE0002,
  NetzError_LinkNotConnected = 0xFE0003,
  NetzError_ProtocolNotPresent = 0xFE0004,
  NetzError_LinkNotOpen = 0xFE0005,
  NetzError_6 = 0xFE0006,
  NetzError_WrongTypeOfLink = 0xFE0007,
  NetzError_InvalidAddress = 0xFE0008,
  NetzError_ResourcePoolEmpty = 0xFE0009,
  NetzError_PacketTooBig = 0xFE000A,
  NetzError_CantCreateNetzEvent = 0xFE000B,
  NetzError_WrongMode = 0xFE000C,
  NetzError_NameNotUnique = 0xFE000D,
  NetzError_Failed = 0xFE000E,
  NetzError_OsLame = 0xFE000F,
  NetzError_Processor = 0xFE0010,
  NetzError_LinkLost = 0xFE0011,
  NetzError_Disabled = 0xFE0012,
  NetzError_NotImplemented = 0xFE0013,
  NetzError_Fatal = 0xFFFFFFFF,
};

/* 398 */
enum KKND::NetzConnectionStatus : unsigned __int8
{
  NetzConnection_None = 0x0,            ///< Free slot
  NetzConnection_Joined = 0x1,          ///< Remote player joined the lobby
  NetzConnection_Local = 0x2,           ///< Local player (self)
  NetzConnection_Synced = 0x3,          ///< Player is confirmed and synced (game starting)
};

/* 364 */
enum KKND::NetzFaction : unsigned __int8
{
  NetzFaction_Surv = 0,
  NetzFaction_Mute = 1,
};

/* 363 */
struct KKND::NetzPlayer
{
  KKND::NetzConnectionStatus connection_status;
  char palette_idx;
  KKND::NetzFaction faction;
  char name[8];
  char __netz_player_field_B;
  char __netz_player_field_C;
  char __netz_player_field_D;
  char __netz_player_field_E;
  char __netz_player_field_F;
  int slot;
  BOOL event_received_this_tick;
  BOOL synced;                          ///< true: player has received & ack'd the latest roster
                                        ///< false: sent roster but no ack yet
};

/* 367 */
struct KKND::ReinforcementsState
{
  void (__fastcall *mode)(KKND::ReinforcementsState *);
  KKND::Task *task;
  unsigned int spawn_x;
  unsigned int spawn_y;
  int player_num;
  int waves_remaining;
  int wave_delay;
  int spawn_table_idx;
  int unit_idx;
  BOOL is_player_allied;
  int enemy_player_num;
};

/* 368 */
struct KKND::Reinforcements
{
  KKND::UnitType *units;
  int trigger_time;
  int wave_delay;
  int waves_remaining;
};

/* 369 */
struct KKND::Briefing
{
  const char **lines;
  const char *wav;
};

/* 373 */
struct KKND::FactoryColorStripe
{
  int palette_idx;
  int sidebar_icon_mobd_frame;
};

/* 383 */
struct KKND::GuardAreaOrderPayload
{
  int player_num;
  int dst_x;
  int dst_y;
};

/* 384 */
struct KKND::KeyBinding
{
  int scancode;
  KKND::KeyboardActions action;
};

/* 386 */
enum KKND::BoxdRaycastPhase : unsigned __int32
{
  RaycastPhase_ScanningClear = 0x0,
  RaycastPhase_InsideObstacle = 0x1,
};

/* 387 */
enum KKND::BoxdRaycastStepResult : unsigned __int32
{
  RaycastStepResult_Continue = 6,
  RaycastStepResult_Stop = 1,
};

/* 388 */
struct KKND::TankerSaveStruct
{
  __int32 oil_loaded;
  __int32 current_destination_unit_id;
  __int32 powerplant_unit_id;
  __int32 drillrig_unit_id;
  __int32 drillrig_unit_id_2;
  __int32 powerplant_unit_id_2;
  __int32 current_destination_unit_id_2;
  __int32 num_destinations;
  __int32 destinations[20];
};

/* 391 */
struct KKND::TechLevelsSaveStruct
{
  __int32 num_buildings_by_level[5];
  __int32 _tech_levels_save_stru_unused_field_10;
  __int32 _tech_levels_save_stru_unused_field_14;
  __int32 _tech_levels_save_stru_unused_field_18;
  __int32 _tech_levels_save_stru_unused_field_1C;
  __int32 max_level;
};

/* 392 */
struct KKND::ConstructSaveStruct
{
  __int32 unit_id;
  __int32 player_num;
  __int32 stage;
  __int32 cost;
  __int32 remaining_cost;
  __int32 cost_per_tick;
};

/* 390 */
struct KKND::MetaSaveStruct
{
  __int32 building_construction_byte_size;
  __int32 num_units_in_group[11];
  __int32 is_building_suspended;
  KKND::TechLevelsSaveStruct outpost;
  KKND::TechLevelsSaveStruct clanhall;
  KKND::TechLevelsSaveStruct machine_shop;
  KKND::TechLevelsSaveStruct beast_enclosure;
  __int32 sidebar_color_bars_used[6];
  __int32 num_player_units;
  __int32 num_ai_units;
  __int32 num_towers;
  __int32 victory_condition_ticks;
  __int32 victory_condition_bits;
  __int32 num_convoy_tankers_en_route;
  __int32 scout_unit_id;
  __int32 player_bases_unit_ids[4];
  __int32 num_ally_waves_remaining;
  __int32 num_enemy_waves_remaining;
  __int32 is_aircraft_unlocked;
  __int32 aircraft_mode_id;
  __int32 num_airstrikes_available;
  KKND::EntitySaveStruct airstrike_counter_entity;
  __int32 aircraft_sidebar_task_channel;
  __int32 aircraft_sidebar_task_id;
  __int32 aircraft_sidebar_task_message_handler;
  __int32 aircraft_sidebar_task_transient_events;
  __int32 aircraft_sidebar_task_sleep;
  __int32 aircraft_sidebar_task_global_events;
  __int32 aircraft_sidebar_task_wait_flags;
  __int32 aircraft_sidebar_task_field_2C;
  __int32 _meta_save_struct_field_174;
  KKND::ConstructSaveStruct constructs[1];
};

/* 393 */
struct KKND::UnitSaveIndex
{
  int unit_id;
  size_t size;
};

/* 395 */
struct KKND::UnitTypeTag
{
  const char *tag;
  KKND::UnitType type;
};

/* 396 */
/// Verified NOT KKND::GameEvent
struct __unaligned KKND::NetzGameEvent
{
  KKND::GameEventType type;
  char payload[12];
};

/* 397 */
enum KKND::MovieType : unsigned __int32
{
  MovieType_Intro = 0x0,
  MovieType_CampaignBriefing = 0x1,
  MovieType_CampaignEnd = 0x2,
};

/* 400 */
struct KKND::CplcPlayerSpawn
{
  int x;
  int y;
  int player_side;
};

/* 401 */
enum KKND::WellKnownMobdIds
{
  MOBD_CURSOR_DEFAULT_ARROW = 0xC,
  MOBD_CURSOR_UNIT_HOVER = 0x30,
  MOBD_CURSOR_DRAG_BOX_X = 0x1CC,
  MOBD_CURSOR_DRAG_BOX_Y = 0x1D8,
  MOBD_CURSOR_DRAG_BOX_Z = 0x1E4,
  MOBD_CURSOR_DRAG_BOX_W = 0x1F0,
  MOBD_CURSOR_HELP_HIGHLIGHT = 0x24,
  MOBD_CURSOR_HELP = 0x18,
  MOBD_CURSOR_MOVE = 0x1C0,
  MOBD_CURSOR_DRILL = 0x23C,
  MOBD_CURSOR_INFILTRATE = 0xF4,
  MOBD_CURSOR_REPAIR = 0x90,
  MOBD_CURSOR_DEPLOY = 0xBC,
  MOBD_CURSOR_UPGRADE = 0xD8,
  MOBD_CURSOR_ATTACK = 0x130,
  MOBD_CURSOR_UNAVAILABLE = 0x124,
  MOBD_CURSOR_MOVE_ACK = 0x1FC,
  MOBD_CURSOR_UPGRADE_CANCEL = 0x118,
};

/* 402 */
enum KKND::NetzPacketType : unsigned __int8
{
  NETZ_PKT_EVENT_BROADCAST = 51,        ///< Host distributes collected game events from all players
  NETZ_PKT_CLIENT_EVENT = 52,           ///< Client submits it's game event for the current lockstep tick (13 bytes if event, 1 if none)
  NETZ_PKT_CLIENT_LOCKSTEP_FINISHED = 59, ///< Client is ready for the next lockstep tick
  NETZ_PKT_GAME_START_LOBBY_STATE_BROADCAST = 50, ///< Full lobby state broadcast (player count, slot, names, palettes, factions) and game start
  NETZ_PKT_CLIENT_ACK = 55,             ///< Generic client ACK
  NETZ_PKT_CLIENT_KICKED_ACK = 56,      ///< Player is kicked notification
  NETZ_PKT_LOBBY_SYNC = 57,             ///< Same as lobby state broadcast but on update
  NETZ_PKT_GAME_OVER_BROADCAST = 62,    ///< Host surrendered / terminated
  NETZ_PKT_PLAYER_REMOVED_BROADCAST = 63, ///< Notify remaining players a player slot was removed
  NETZ_PKT_GAME_SETTINGS_SYNC = 64,     ///< Game settings update/sync (tech level, map etc)
  NETZ_PKT_LOBBY_PLAYERS_BROADCAST = 65, ///< Player list (names, palettes, factions) refresh
  NETZ_PKT_LOBBY_PLAYER_JOINED = 66,
  NETZ_PKT_LOBBY_GAME_CANCELLED_BROADCAST = 67, ///< Host cancelled game / closed session
  NETZ_PKT_LOBBY_RESOLVE_PALETTE_CONFLICT = 69, ///< Find another player with the same palette, reassign
  NETZ_PKT_LOBBY_READY_TO_START = 70,   ///< Client is ready to start
  NETZ_PKT_LOBBY_START_WITH_SEED = 71,  ///< Game is starting woth g_rand_seed_sync. Client ACK with 55
  NETZ_PKT_JOIN_LOCAL = 72,
  NETZ_PKT_CHAT = 73,                   ///< Sent to host, host relayes to other players
  NETZ_PKT_74 = 74,
  NETZ_PKT_75 = 75,
  NETZ_PKT_GAME_OVER_BROADCAST_2 = 61,
  NETZ_PKT_JOIN_REQ = 62,               ///< Player sends a join request along with own NetzPlayer data
  NETZ_PKT_REJECT = 76,                 ///< Host rejects connection
  NETZ_PKT_JOIN_REQ_WITH_VERSION = 31,
  NETZ_PKT_BROADCAST_PLAYER_KICKED = 68,
  NETZ_PKT_1E = 30,
};

/* 404 */
enum KKND::NetzLinkConnectionState : unsigned __int32
{
  NetzLinkConnectionState_None = 0,
  NetzLinkConnectionState_Connected = 2,
};

/* 403 */
struct KKND::NetzLink
{
  KKND::NetzLinkConnectionState connection_state;
  int _netz_link_field_4[8];
  DPID dpid;
  BOOL is_active;
};

/* 406 */
struct KKND::NetzSendBuffer
{
  uint8_t recv_seq;
  uint8_t send_seq;
  uint8_t _netz_send_buffer_field_2;
  KKND::NetzPacketType pkt;
  uint8_t buf[280];
  int last_send_size;
};

/* 407 */
struct __unaligned KKND::KaosSettings
{
  int settings;
  char _kaos_settings_field_4;
};

/* 408 */
struct KKND::FollowOrderPayload
{
  int player_num;
  KKND::Unit *target;
};

/* 409 */
struct KKND::DplaySession
{
  GUID session_guid;
  int num_current_players;
  int max_players;
  int flags;
  char session_name[16];
  KKND::DplaySession *next;
};

/* 410 */
struct KKND::NetzRosterPlayer
{
  uint8_t present;
  uint8_t name[12];
  uint8_t palette;
  uint8_t faction;
};

/* 411 */
struct KKND::NetzRoster
{
  unsigned __int32 num_players;
  unsigned __int32 slot;
  KKND::NetzRosterPlayer players[6];    ///< sender's slot or recipient's assigned slot
};

/* 413 */
enum KKND::NetzJoinState : __int32
{
  NetzJoinState_Idle = -2,
  NetzJoinState_Connecting = -1,
  NetzJoinState_Synced = 0,
  NetzJoinState_Rejected = 1,
};

/* 414 */
struct KKND::NetzProvider
{
  int _netz_provider_field_0;
  const char *names[3];
  int _netz_provider_field_10;
  int _netz_provider_field_14[2];
  void *vtbl[19];
};

/* 415 */
struct KKND::NetzTimer
{
  BOOL active;
  KKND::NetzTimer *next;
  int _netz_timer_field_8[2];
  int fire_at;
  int retries;
  void (__fastcall *handler)();
};

/* 416 */
struct __unaligned KKND::NetzJoinPkt
{
  uint8_t name[7];
  uint8_t palette;
  uint32_t flags;
  uint8_t build_date[12];
  uint8_t build_time[9];
};

/* 418 */
struct KKND::DplayPlayer
{
  char short_name[16];
  char long_name[16];
  DPID player_id;
  KKND::DplayPlayer *next;
};

/* 422 */
struct KKND::SaveSlot
{
  char name[20];
  KKND::LevelId level_id;
};

/* 426 */
struct KKND::NetzMobemPhonebook
{
  KKND::NetzMobemPhonebook *next;
  KKND::NetzMobemPhonebook *prev;
  char name[12];
  char phone[12];
  int baud_index;
};

