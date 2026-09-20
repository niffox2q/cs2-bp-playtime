#pragma once

#include <functional>
#include <string>
#include <map>

class IMySQLConnection;

struct Task
{
    std::string szIdentifier;
    std::string szName;
    std::string szDescription;
    std::string szEvent;
    std::map<std::string, std::string> vecParamsString;
    std::map<std::string, int> vecParamsInt;
	std::map<std::string, bool> vecParamsBool;
    std::string szCount  = "1";
    int iNeeded;
    int iEXP;
    int iTokens;
    bool bPaid;
};

struct CategoryTask
{
	std::string szName;
	int iTime;
	int iCountGive;
	int iCountGivePaid;
	std::map<std::string, Task> vecTasks;
};

struct Progress
{
	int iID;
	std::string sMission;
	int iProgress;
	int iNeeded;
	int iExpires;
};

struct UserData
{
	int iID;
	int iLevel;
	int iEXP;
	int iTokens;
	bool bPaid;
	std::map<std::string, Progress> vecProgress;
	std::vector<int> vecLevelsProgress;
};

struct Season
{
	int iID;
	std::string sName;
	int iStartDate;
	int iEndDate;
};

struct Levels
{
	int iLevel;
	int iEXP;
	int iTokens;
	bool bPaid;
};

struct ShopItem
{
	std::string szIdentifier;
	std::string szName;
	std::string szDescription;
	int iPrice;
	bool bPaid;
	std::string szType;
	std::map<std::string, std::string> vecParams;
};

struct CategoryShop
{
	std::string szIdentifier;
	std::string szName;

	std::unordered_map<std::string, ShopItem> vecItems;
};

typedef std::function<void()> OnCoreLoadCallback;
typedef std::function<void(int iSlot, const char* szType, std::map<std::string, std::string> vecParams)> OnSendShopTypeCallback;

#define BP_INTERFACE "IBattlePassApi"
class IBattlePassApi
{
public:
	virtual IMySQLConnection* GetConnection() = 0;
    virtual std::map<int, Levels> GetLevels() = 0;

    virtual Season GetSeason() = 0;
    virtual std::vector<CategoryTask> GetTasks() = 0;
    virtual Task GetTask(const char* szIdentifier) = 0;
    virtual UserData GetUser(int iSlot) = 0;
    virtual void SendCustomEvent(int iSlot, const char* szEvent, std::map<std::string, std::string> vecParamsString, std::map<std::string, int> vecParamsInt) = 0;

	virtual bool IsCoreLoaded() = 0;
	virtual void HookOnCoreLoad(SourceMM::PluginId id, OnCoreLoadCallback callback) = 0;

	virtual void SendShopType(int iSlot, const char* szType, std::map<std::string, std::string> vecParams) = 0;
	virtual void HookSendShopType(SourceMM::PluginId id, OnSendShopTypeCallback callback) = 0;
};