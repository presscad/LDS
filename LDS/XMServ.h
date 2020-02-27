//
// sproxy.exe generated file
// do not modify this file
//
// Created: 08/16/2012@16:30:29
//

#pragma once


#ifndef _WIN32_WINDOWS
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace XMServer
{

struct Function
{
	int functionId;
	BSTR functdesc;
};

struct RelationObject
{
	int relationId;
	BSTR relationName;
	BSTR relationObj;
};

struct TestItem
{
	int id;
	bool share;
	BSTR title;
	BSTR description;
	int ProductFlag;
	RelationObject categoryObj;
};

struct DevelopLog
{
	int id;
	BSTR title;
	BSTR description;
	BSTR startDate;
	BSTR endDate;
	RelationObject user;
	RelationObject production;
	RelationObject relateFile;
	TestItem *testItemArr;
	int __testItemArr_nSizeIs;
	BSTR tag;
};

struct WorLog_testItem
{
	int logId;
	int testId;
	bool processed;
};

struct Attendance
{
	int id;
	RelationObject user;
	RelationObject supervisor;
	BSTR submitDate;
	BSTR attendanceDate;
	int attendanceType;
	BSTR mender;
	BSTR notes;
};

struct FileHeader
{
	BSTR filePath;
	int id;
	int fileType;
	BSTR fileName;
	BSTR fileNameServer;
	int hostId;
	int ownerId;
	ATLSOAP_BLOB matter;
	BSTR description;
	BSTR tag;
};

struct Reply
{
	int id;
	RelationObject feedBack;
	BSTR content;
	RelationObject processMan;
	FileHeader *attachmFileArr;
	int __attachmFileArr_nSizeIs;
	BSTR processDate;
	BSTR tag;
};

struct Client_DownloadItem
{
	int client_id;
	int downloadItem_id;
};

struct Bug
{
	int bugId;
	BSTR bugDesc;
};

struct VersionInfo
{
	int versionId;
	BSTR version;
	BSTR productionName;
	BSTR promulgateDate;
	Function *addFunction;
	int __addFunction_nSizeIs;
	Bug *addBug;
	int __addBug_nSizeIs;
};

struct DownloadItem
{
	int id;
	BSTR file_name;
	BSTR title;
	BSTR updateDate;
	BSTR notes;
	FileHeader file;
};

struct TaskDispatch
{
	int id;
	RelationObject acceptUser;
	RelationObject promulgator;
	BSTR title;
	BSTR content;
	int taskState;
	BSTR dispatchDate;
	BSTR updateDate;
	FileHeader *relativeFileArr;
	int __relativeFileArr_nSizeIs;
	BSTR reply;
	BSTR notes;
	BSTR remark;
	bool remarkVisible;
	bool isGroup;
	int groupId;
	int categoryId;
	BSTR tag;
	int serialNo;
	int productFlag;
};

struct Role
{
	int id;
	BSTR roleName;
	ATLSOAP_BLOB funcOperPermBytes;
	BSTR notes;
	BSTR tag;
};

struct Production
{
	int id;
	int userId;
	BSTR name;
	BSTR notes;
	BSTR Tag;
};

struct User
{
	int id;
	RelationObject client;
	BSTR userName;
	BSTR password;
	BSTR reallyName;
	BSTR sex;
	BSTR rank;
	BSTR email;
	BSTR nowTask;
	BSTR taskSubmitDate;
	BSTR enrolTime;
	int dWord;
	BSTR notes;
	Role *roleArr;
	int __roleArr_nSizeIs;
	Production *productArr;
	int __productArr_nSizeIs;
	BSTR tag;
};

struct Client
{
	int id;
	BSTR unitName;
	BSTR address;
	BSTR linkman;
	BSTR telephone;
	BSTR fax;
	int province;
	BSTR postalCode;
	BSTR enrollTime;
	Production *productionArr;
	int __productionArr_nSizeIs;
	DownloadItem *downloadItemArr;
	int __downloadItemArr_nSizeIs;
};

struct LoginClass
{
	int sessionId;
	User user;
	BSTR tag;
};

struct PartBuild
{
	int id;
	BSTR name;
	BSTR place;
	int partItemId;
	BSTR submitDate;
};

struct TestItemSet
{
	int id;
	BSTR title;
	ATLSOAP_BLOB proByte;
	int categoryId;
	BSTR submit_date;
	BSTR notes;
};

struct Category
{
	int id;
	int objType;
	BSTR name;
	int parentId;
	ATLSOAP_BLOB proByte;
};

struct Client_Production
{
	int client_id;
	int production_id;
	BSTR tag;
};

struct XMSession
{
	int sessionId;
	Role *RoleArr;
	int __RoleArr_nSizeIs;
};

struct LoginStruct
{
	int sessionId;
	User user;
	XMSession XMS;
};

struct WorkLog
{
	int id;
	bool hasTestItem;
	BSTR description;
	BSTR submitDate;
	RelationObject user;
	BSTR title;
	BSTR notes;
	BSTR startDate;
	BSTR endDate;
	TestItem *testItemArr;
	int __testItemArr_nSizeIs;
};

struct Version_testItem
{
	RelationObject version;
	RelationObject testItem;
	bool processed;
	BSTR notes;
	bool result;
	RelationObject user;
	BSTR update_date;
};

struct Job
{
	int id;
	BSTR title;
	BSTR content;
	RelationObject promulgator;
	BSTR date;
};

struct VersionRevision
{
	int id;
	BSTR version;
	RelationObject product;
	BSTR promulgateDate;
	RelationObject promulgator;
	FileHeader *relativeFileArr;
	int __relativeFileArr_nSizeIs;
	BSTR descriptionInner;
	BSTR descriptionOut;
	BSTR tag;
};

struct News
{
	int id;
	BSTR title;
	BSTR content;
	BSTR issueDate;
	RelationObject promulgator;
};

struct Notification
{
	int id;
	int receiverFlagWord;
	BSTR title;
	BSTR content;
	BSTR promulgateDate;
	RelationObject promulgator;
	FileHeader *relativeFileArr;
	int __relativeFileArr_nSizeIs;
	BSTR notes;
	BSTR tag;
};

struct ProblemGrade
{
	int id;
	BSTR gradeName;
	BSTR notes;
};

struct FeedBack
{
	int id;
	BSTR title;
	BSTR version;
	BSTR content;
	int scanNum;
	RelationObject production;
	RelationObject problemGrade;
	RelationObject proposer;
	BSTR submitDate;
	BSTR updateDate;
	bool isEssential;
	bool group;
	int superiorId;
	BSTR state;
	BSTR problemtype;
	FileHeader *relativeFileArr;
	int __relativeFileArr_nSizeIs;
	Reply *replyArr;
	int __replyArr_nSizeIs;
	int serialNo;
	BSTR tag;
	int shadowSuperiorId;
};

template <typename TClient = CSoapSocketClientT<> >
class CXMServerT : 
	public TClient, 
	public CSoapRootHandler
{
protected:

	const _soapmap ** GetFunctionMap();
	const _soapmap ** GetHeaderMap();
	void * GetHeaderValue();
	const wchar_t * GetNamespaceUri();
	const char * GetServiceName();
	const char * GetNamespaceUriA();
	HRESULT CallFunction(
		void *pvParam, 
		const wchar_t *wszLocalName, int cchLocalName,
		size_t nItem);
	HRESULT GetClientReader(ISAXXMLReader **ppReader);

public:

	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv)
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_ISAXContentHandler))
		{
			*ppv = static_cast<ISAXContentHandler *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef()
	{
		return 1;
	}

	ULONG __stdcall Release()
	{
		return 1;
	}

	CXMServerT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://localhost/XMService/XMServer.asmx"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CXMServerT()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT GetUser(
		User* GetUserResult
	);

	HRESULT DownloadNotification(
		int sessionId, 
		int id, 
		Notification* DownloadNotificationResult
	);

	HRESULT SaveOrUpdateTaskDispatch(
		int sessionId, 
		TaskDispatch taskDispatch, 
		bool isFile, 
		int* SaveOrUpdateTaskDispatchResult
	);

	HRESULT SaveOrUpdateDownloadItem(
		int sessionId, 
		DownloadItem item, 
		int* SaveOrUpdateDownloadItemResult
	);

	HRESULT QueryFeedBackArrByProblemType(
		int sessionId, 
		int problemType, 
		FeedBack** QueryFeedBackArrByProblemTypeResult, int* QueryFeedBackArrByProblemTypeResult_nSizeIs
	);

	HRESULT QueryProblemGradeArr(
		int sessionId, 
		ProblemGrade** QueryProblemGradeArrResult, int* QueryProblemGradeArrResult_nSizeIs
	);

	HRESULT UpdateUser(
		int sessionId, 
		User user, 
		bool isRole, 
		bool isProduct, 
		bool* UpdateUserResult
	);

	HRESULT DownLoadNews(
		int sessionId, 
		int id, 
		News* DownLoadNewsResult
	);

	HRESULT QueryProductionArr(
		int sessionId, 
		Production** QueryProductionArrResult, int* QueryProductionArrResult_nSizeIs
	);

	HRESULT QueryAllRole(
		int sessionId, 
		Role** QueryAllRoleResult, int* QueryAllRoleResult_nSizeIs
	);

	HRESULT InsertUser(
		int sessionId, 
		User newUser, 
		int* InsertUserResult
	);

	HRESULT UpdateUserTask(
		int sessionId, 
		int userId, 
		BSTR nowTask, 
		bool* UpdateUserTaskResult
	);

	HRESULT QueryChildTaskDispatchsByGroupId(
		int sessionId, 
		int groupId, 
		TaskDispatch** QueryChildTaskDispatchsByGroupIdResult, int* QueryChildTaskDispatchsByGroupIdResult_nSizeIs
	);

	HRESULT UpdateTestItemArr(
		int sessionId, 
		TestItem* tiArr, int tiArr_nSizeIs, 
		bool* UpdateTestItemArrResult
	);

	HRESULT GetVersionRevision(
		VersionRevision* GetVersionRevisionResult
	);

	HRESULT DeleteCategoryById(
		int sessionId, 
		int id, 
		bool* DeleteCategoryByIdResult
	);

	HRESULT QueryVersionArr(
		int sessionId, 
		int productionId, 
		BSTR startDate, 
		BSTR endDate, 
		BSTR keyword, 
		VersionRevision** QueryVersionArrResult, int* QueryVersionArrResult_nSizeIs
	);

	HRESULT QueryTestItemSetArrByProId(
		int sessionId, 
		int proId, 
		TestItemSet** QueryTestItemSetArrByProIdResult, int* QueryTestItemSetArrByProIdResult_nSizeIs
	);

	HRESULT GetFileHeader(
		FileHeader* GetFileHeaderResult
	);

	HRESULT GetReply(
		Reply* GetReplyResult
	);

	HRESULT ValidateUserPerm(
		int sessionId, 
		int permOper, 
		bool* ValidateUserPermResult
	);

	HRESULT UpdateTaskDispatchArr(
		int sessionId, 
		TaskDispatch* taskArr, int taskArr_nSizeIs, 
		int** UpdateTaskDispatchArrResult, int* UpdateTaskDispatchArrResult_nSizeIs
	);

	HRESULT SaveOrUpdateProduction(
		int sessionId, 
		Production product, 
		int* SaveOrUpdateProductionResult
	);

	HRESULT InsertFeedBackArr(
		int sessionId, 
		FeedBack* feedBackArr, int feedBackArr_nSizeIs, 
		bool* InsertFeedBackArrResult
	);

	HRESULT DownloadProduction(
		int sessionId, 
		int id, 
		Production* DownloadProductionResult
	);

	HRESULT DeleteWorkLog(
		int sessionId, 
		int logId, 
		bool* DeleteWorkLogResult
	);

	HRESULT QueryTestItemArrBySetId(
		int sessionId, 
		int setId, 
		bool share, 
		TestItem** QueryTestItemArrBySetIdResult, int* QueryTestItemArrBySetIdResult_nSizeIs
	);

	HRESULT QueryClientProductByClientId(
		int sessionId, 
		int clientId, 
		Client_Production** QueryClientProductByClientIdResult, int* QueryClientProductByClientIdResult_nSizeIs
	);

	HRESULT QueryProductionByUserId(
		int sessionId, 
		int userId, 
		Production** QueryProductionByUserIdResult, int* QueryProductionByUserIdResult_nSizeIs
	);

	HRESULT QueryTestItemArrByCondition(
		int sessionId, 
		int proId, 
		int categoryId, 
		bool share, 
		TestItem** QueryTestItemArrByConditionResult, int* QueryTestItemArrByConditionResult_nSizeIs
	);

	HRESULT QueryproductionsByClientId(
		int sessionId, 
		int clientId, 
		Production** QueryproductionsByClientIdResult, int* QueryproductionsByClientIdResult_nSizeIs
	);

	HRESULT DownloadFeedBack(
		int sessionId, 
		int id, 
		FeedBack* DownloadFeedBackResult
	);

	HRESULT GetWorkLog(
		WorkLog* GetWorkLogResult
	);

	HRESULT DeleteClientById(
		int sessionId, 
		int id, 
		bool* DeleteClientByIdResult
	);

	HRESULT DeleteTaskDispatch(
		int sessionId, 
		int id, 
		bool isGroup, 
		bool* DeleteTaskDispatchResult
	);

	HRESULT GetRole(
		Role* GetRoleResult
	);

	HRESULT QueryDownloadItemByClient(
		int sessionId, 
		int clientId, 
		DownloadItem** QueryDownloadItemByClientResult, int* QueryDownloadItemByClientResult_nSizeIs
	);

	HRESULT DeleteDownloadItem(
		int sessionId, 
		int id, 
		bool* DeleteDownloadItemResult
	);

	HRESULT DeleteClient_DownloadItemByItem(
		int itemId, 
		bool* DeleteClient_DownloadItemByItemResult
	);

	HRESULT QueryEssentialFeedBacks(
		int productId, 
		FeedBack** QueryEssentialFeedBacksResult, int* QueryEssentialFeedBacksResult_nSizeIs
	);

	HRESULT QueryFeedBackArrByProcessState(
		int sessionId, 
		bool state, 
		FeedBack** QueryFeedBackArrByProcessStateResult, int* QueryFeedBackArrByProcessStateResult_nSizeIs
	);

	HRESULT QueryCategoryArrByObjType(
		int sessionId, 
		int objType, 
		Category** QueryCategoryArrByObjTypeResult, int* QueryCategoryArrByObjTypeResult_nSizeIs
	);

	HRESULT SaveOrUpdateJob(
		int sessionId, 
		Job job, 
		int* SaveOrUpdateJobResult
	);

	HRESULT QueryFeedBacksToDataSet(
		int sessionId, 
		int promulgatorId, 
		bool isUnSolve, 
		bool isSolve, 
		bool isGroup, 
		BSTR* QueryFeedBacksToDataSetResult
	);

	HRESULT SaveOrUpdateNotification(
		int sessionId, 
		Notification notification, 
		bool isFile, 
		int* SaveOrUpdateNotificationResult
	);

	HRESULT InsertUser_Role(
		int sessionId, 
		int userId, 
		Role* roleArr, int roleArr_nSizeIs, 
		bool* InsertUser_RoleResult
	);

	HRESULT SaveOrUpdateVersionRevision(
		int sessionId, 
		VersionRevision version, 
		int* SaveOrUpdateVersionRevisionResult
	);

	HRESULT QueryTestItemByWorkLogDate(
		int sessionId, 
		BSTR startDate, 
		BSTR endDate, 
		TestItem** QueryTestItemByWorkLogDateResult, int* QueryTestItemByWorkLogDateResult_nSizeIs
	);

	HRESULT QueryFeedBackArrByName(
		int sessionId, 
		BSTR name, 
		FeedBack** QueryFeedBackArrByNameResult, int* QueryFeedBackArrByNameResult_nSizeIs
	);

	HRESULT DeleteUser(
		int sessionId, 
		int id, 
		bool* DeleteUserResult
	);

	HRESULT SaveOrUpdateCategory(
		int sessionId, 
		Category category, 
		int* SaveOrUpdateCategoryResult
	);

	HRESULT DownloadClientProductById(
		int id, 
		Client_Production* DownloadClientProductByIdResult
	);

	HRESULT QueryLatestVersionRevisionArr(
		int sessionId, 
		VersionRevision** QueryLatestVersionRevisionArrResult, int* QueryLatestVersionRevisionArrResult_nSizeIs
	);

	HRESULT DeleteReply(
		int sessionId, 
		int id, 
		bool* DeleteReplyResult
	);

	HRESULT QueryTaskDispatchs(
		int sessionId, 
		BSTR keyword, 
		int promulgatorId, 
		int acceptUserId, 
		int task_state, 
		BSTR startDate, 
		BSTR endDate, 
		int taskProId, 
		TaskDispatch** QueryTaskDispatchsResult, int* QueryTaskDispatchsResult_nSizeIs
	);

	HRESULT DownloadAttendanceById(
		int sessionId, 
		int id, 
		Attendance* DownloadAttendanceByIdResult
	);

	HRESULT DownloadCategoryById(
		int sessionId, 
		int id, 
		Category* DownloadCategoryByIdResult
	);

	HRESULT QueryDevelopLogArr(
		int sessionId, 
		int* productIdArr, int productIdArr_nSizeIs, 
		int userId, 
		BSTR startDate, 
		BSTR endDate, 
		DevelopLog** QueryDevelopLogArrResult, int* QueryDevelopLogArrResult_nSizeIs
	);

	HRESULT DownloadItemById(
		int id, 
		DownloadItem* DownloadItemByIdResult
	);

	HRESULT InsertTaskDispatchArr(
		int sessionId, 
		TaskDispatch* taskArr, int taskArr_nSizeIs, 
		int** InsertTaskDispatchArrResult, int* InsertTaskDispatchArrResult_nSizeIs
	);

	HRESULT DownLoadLasrVersionByProId(
		int sessionId, 
		int productionId, 
		VersionRevision* DownLoadLasrVersionByProIdResult
	);

	HRESULT QueryClientByProvinceNum(
		int sessionId, 
		int num, 
		Client** QueryClientByProvinceNumResult, int* QueryClientByProvinceNumResult_nSizeIs
	);

	HRESULT DeleteWorkLogById(
		int sessionId, 
		int id, 
		bool* DeleteWorkLogByIdResult
	);

	HRESULT QueryFeedBackArrByProposerId(
		int sessionId, 
		int proposerId, 
		FeedBack** QueryFeedBackArrByProposerIdResult, int* QueryFeedBackArrByProposerIdResult_nSizeIs
	);

	HRESULT DownLoadJobById(
		int sessionId, 
		int id, 
		Job* DownLoadJobByIdResult
	);

	HRESULT SaveOrUpdateTestItem(
		int sessionId, 
		TestItem ti, 
		int* SaveOrUpdateTestItemResult
	);

	HRESULT QueryAllJobs(
		int sessionId, 
		Job** QueryAllJobsResult, int* QueryAllJobsResult_nSizeIs
	);

	HRESULT QueryPartBuildByPartItemId(
		int partItemId, 
		PartBuild** QueryPartBuildByPartItemIdResult, int* QueryPartBuildByPartItemIdResult_nSizeIs
	);

	HRESULT QueryRolesByUserId(
		int userId, 
		Role** QueryRolesByUserIdResult, int* QueryRolesByUserIdResult_nSizeIs
	);

	HRESULT QueryFeedBackArrByProblemGrade(
		int sessionId, 
		int problemGrade, 
		FeedBack** QueryFeedBackArrByProblemGradeResult, int* QueryFeedBackArrByProblemGradeResult_nSizeIs
	);

	HRESULT DeleteTestItemSet(
		int sessionId, 
		int id, 
		bool* DeleteTestItemSetResult
	);

	HRESULT QueryFeedBackArrByCondition(
		int sessionId, 
		BSTR keyword, 
		int gradeId, 
		BSTR problemType, 
		BSTR processState, 
		int* productIdArr, int productIdArr_nSizeIs, 
		FeedBack** QueryFeedBackArrByConditionResult, int* QueryFeedBackArrByConditionResult_nSizeIs
	);

	HRESULT DownloadVersionRevision(
		int sessionId, 
		int id, 
		VersionRevision* DownloadVersionRevisionResult
	);

	HRESULT DeleteAttendance(
		int sessionId, 
		int id, 
		bool* DeleteAttendanceResult
	);

	HRESULT ValidateUserName(
		BSTR userName, 
		bool* ValidateUserNameResult
	);

	HRESULT DeleteTestItemById(
		int sessionId, 
		int id, 
		bool* DeleteTestItemByIdResult
	);

	HRESULT DownloadTaskDispatch(
		int sessionId, 
		int id, 
		TaskDispatch* DownloadTaskDispatchResult
	);

	HRESULT QueryFeedBacksCount(
		int sessionId, 
		int gradeId, 
		BSTR problemType, 
		BSTR processState, 
		int* productIdArr, int productIdArr_nSizeIs, 
		int* QueryFeedBacksCountResult
	);

	HRESULT QueryInsideUsers(
		int sessionId, 
		User** QueryInsideUsersResult, int* QueryInsideUsersResult_nSizeIs
	);

	HRESULT GetLoginStruct(
		LoginStruct* GetLoginStructResult
	);

	HRESULT QueryTestItemArr(
		int sessionId, 
		TestItem** QueryTestItemArrResult, int* QueryTestItemArrResult_nSizeIs
	);

	HRESULT DeleteNotification(
		int sessionId, 
		int id, 
		bool* DeleteNotificationResult
	);

	HRESULT SaveOrUpdateNews(
		int sessionId, 
		News news, 
		int* SaveOrUpdateNewsResult
	);

	HRESULT QueryVersionRevisionArrByProductionId(
		int sessionId, 
		int productionId, 
		VersionRevision** QueryVersionRevisionArrByProductionIdResult, int* QueryVersionRevisionArrByProductionIdResult_nSizeIs
	);

	HRESULT QueryFeedBackArrByProductionId(
		int sessionId, 
		int productionId, 
		FeedBack** QueryFeedBackArrByProductionIdResult, int* QueryFeedBackArrByProductionIdResult_nSizeIs
	);

	HRESULT DeleteDeveLop_testItemByDevelopLopId(
		int sessionId, 
		int developLogId, 
		bool* DeleteDeveLop_testItemByDevelopLopIdResult
	);

	HRESULT DeleteClient_DownloadItemByClient(
		int clientId, 
		bool* DeleteClient_DownloadItemByClientResult
	);

	HRESULT QueryAttendanceByDate(
		int sessionId, 
		BSTR startDate, 
		BSTR endDate, 
		Attendance** QueryAttendanceByDateResult, int* QueryAttendanceByDateResult_nSizeIs
	);

	HRESULT GetTaskDispatch(
		TaskDispatch* GetTaskDispatchResult
	);

	HRESULT DeleteRole(
		int sessionId, 
		int id, 
		bool* DeleteRoleResult
	);

	HRESULT UpdateVersion_TestItemArr(
		int sessionId, 
		Version_testItem* vtArr, int vtArr_nSizeIs, 
		bool* UpdateVersion_TestItemArrResult
	);

	HRESULT SaveOrUpdateClient(
		int sessionId, 
		Client client, 
		bool isProduct, 
		bool isDownloadItem, 
		int* SaveOrUpdateClientResult
	);

	HRESULT QueryAllDevelopLog(
		int sessionId, 
		DevelopLog** QueryAllDevelopLogResult, int* QueryAllDevelopLogResult_nSizeIs
	);

	HRESULT UpdateFeedBacksSerialNo(
		int sessionId, 
		int* idArr, int idArr_nSizeIs, 
		int* serialArr, int serialArr_nSizeIs, 
		bool* UpdateFeedBacksSerialNoResult
	);

	HRESULT DownloadFileHeader(
		int sessionId, 
		int id, 
		FileHeader* DownloadFileHeaderResult
	);

	HRESULT QueryLikeClients(
		int sessionId, 
		BSTR keyword, 
		bool isProduct, 
		bool isDownloadItem, 
		Client** QueryLikeClientsResult, int* QueryLikeClientsResult_nSizeIs
	);

	HRESULT QueryWorkLogByDateAndProId(
		int sessionId, 
		BSTR startDate, 
		BSTR endDate, 
		int proId, 
		WorkLog** QueryWorkLogByDateAndProIdResult, int* QueryWorkLogByDateAndProIdResult_nSizeIs
	);

	HRESULT DownloadPartBuildById(
		int id, 
		PartBuild* DownloadPartBuildByIdResult
	);

	HRESULT LogoutUser(
		int sessionId, 
		bool* LogoutUserResult
	);

	HRESULT QueryLastReplyByFeedBackId(
		int sessionId, 
		int feedBackId, 
		Reply* QueryLastReplyByFeedBackIdResult
	);

	HRESULT DownLoadClientById(
		int sessionId, 
		int id, 
		Client* DownLoadClientByIdResult
	);

	HRESULT QueryGroupFeedBacks(
		int sessionId, 
		int superiorId, 
		FeedBack** QueryGroupFeedBacksResult, int* QueryGroupFeedBacksResult_nSizeIs
	);

	HRESULT SaveOrUpdateFeedbackGroup(
		int sessionId, 
		FeedBack* feedbackArr, int feedbackArr_nSizeIs, 
		int** SaveOrUpdateFeedbackGroupResult, int* SaveOrUpdateFeedbackGroupResult_nSizeIs
	);

	HRESULT QueryUserByRoleId(
		int sessionId, 
		int roleId, 
		User** QueryUserByRoleIdResult, int* QueryUserByRoleIdResult_nSizeIs
	);

	HRESULT DeleteClient_ProductionByProduct(
		int productId, 
		bool* DeleteClient_ProductionByProductResult
	);

	HRESULT SaveOrUpdateFeedBack(
		int sessionId, 
		FeedBack feedBack, 
		bool isFile, 
		bool isRevert, 
		int* SaveOrUpdateFeedBackResult
	);

	HRESULT DeleteUser_RoleByUserIdAndRoleId(
		int sessionId, 
		int userId, 
		int roleId, 
		bool* DeleteUser_RoleByUserIdAndRoleIdResult
	);

	HRESULT SaveOrUpdateTestItemSet(
		int sessionId, 
		TestItemSet testSet, 
		int* tiIdArr, int tiIdArr_nSizeIs, 
		int* SaveOrUpdateTestItemSetResult
	);

	HRESULT ClearUserPassword(
		int userId, 
		bool* ClearUserPasswordResult
	);

	HRESULT QueryAllAttendance(
		Attendance** QueryAllAttendanceResult, int* QueryAllAttendanceResult_nSizeIs
	);

	HRESULT DeleteDeveLop_testItemByItemId(
		int sessionId, 
		int itemId, 
		bool* DeleteDeveLop_testItemByItemIdResult
	);

	HRESULT QueryWorkLogByUserId(
		int sessionId, 
		int userId, 
		WorkLog** QueryWorkLogByUserIdResult, int* QueryWorkLogByUserIdResult_nSizeIs
	);

	HRESULT QueryFeedBackArrBySubmitDate(
		int sessionId, 
		BSTR startDate, 
		BSTR endDate, 
		FeedBack** QueryFeedBackArrBySubmitDateResult, int* QueryFeedBackArrBySubmitDateResult_nSizeIs
	);

	HRESULT DownloadTestItemById(
		int sessionId, 
		int id, 
		TestItem* DownloadTestItemByIdResult
	);

	HRESULT ValidatePassword(
		int sessionId, 
		int userId, 
		BSTR password, 
		bool* ValidatePasswordResult
	);

	HRESULT QueryTestItemByWorkLogId(
		int sessionId, 
		int logId, 
		TestItem** QueryTestItemByWorkLogIdResult, int* QueryTestItemByWorkLogIdResult_nSizeIs
	);

	HRESULT QueryNotificationArr(
		int sessionId, 
		BSTR startDate, 
		BSTR endDate, 
		Notification** QueryNotificationArrResult, int* QueryNotificationArrResult_nSizeIs
	);

	HRESULT GetRelationObject(
		RelationObject* GetRelationObjectResult
	);

	HRESULT DeletePartBuildByPartItemId(
		int partItemId, 
		bool* DeletePartBuildByPartItemIdResult
	);

	HRESULT DownLoadMemberRole(
		int sessionId, 
		Role* DownLoadMemberRoleResult
	);

	HRESULT DeleteProblemGradeById(
		int sessionId, 
		int id, 
		bool* DeleteProblemGradeByIdResult
	);

	HRESULT QueryAllDownloadItem(
		int sessionId, 
		DownloadItem** QueryAllDownloadItemResult, int* QueryAllDownloadItemResult_nSizeIs
	);

	HRESULT QueryAllNews(
		int sessionId, 
		News** QueryAllNewsResult, int* QueryAllNewsResult_nSizeIs
	);

	HRESULT GetNotification(
		Notification* GetNotificationResult
	);

	HRESULT SaveOrUpdateDevelopLog(
		int sessionId, 
		DevelopLog developLog, 
		int* SaveOrUpdateDevelopLogResult
	);

	HRESULT QueryClientArr(
		int sessionId, 
		bool isProduct, 
		bool isDownloadItem, 
		Client** QueryClientArrResult, int* QueryClientArrResult_nSizeIs
	);

	HRESULT SaveOrUpdateWorkLog(
		int sessionId, 
		WorkLog workLog, 
		int* SaveOrUpdateWorkLogResult
	);

	HRESULT QueryCreateTaskUsers(
		int sessionId, 
		User** QueryCreateTaskUsersResult, int* QueryCreateTaskUsersResult_nSizeIs
	);

	HRESULT DeleteClient_ProductionByClient(
		int clientId, 
		bool* DeleteClient_ProductionByClientResult
	);

	HRESULT SaveOrUpdateReply(
		int sessionId, 
		Reply reply, 
		bool isFile, 
		int* SaveOrUpdateReplyResult
	);

	HRESULT DeleteProduction(
		int sessionId, 
		int productId, 
		bool* DeleteProductionResult
	);

	HRESULT GetProduction(
		Production* GetProductionResult
	);

	HRESULT DownLoadTechnoiogyRole(
		int sessionId, 
		Role* DownLoadTechnoiogyRoleResult
	);

	HRESULT QueryFeedBackArrByVersionRevision(
		int sessionId, 
		int versionRevisionId, 
		FeedBack** QueryFeedBackArrByVersionRevisionResult, int* QueryFeedBackArrByVersionRevisionResult_nSizeIs
	);

	HRESULT SaveOrUpdateAttendance(
		int sessionId, 
		Attendance attendance, 
		int* SaveOrUpdateAttendanceResult
	);

	HRESULT DeleteClientDownloadByClientId(
		int sessionId, 
		int id, 
		bool* DeleteClientDownloadByClientIdResult
	);

	HRESULT SaveOrUpdateRole(
		int sessionId, 
		Role role, 
		int* SaveOrUpdateRoleResult
	);

	HRESULT QueryAllFeedBackArr(
		int sessionId, 
		FeedBack** QueryAllFeedBackArrResult, int* QueryAllFeedBackArrResult_nSizeIs
	);

	HRESULT DeleteFeedBackGroup(
		int sessionId, 
		int groupId, 
		bool* DeleteFeedBackGroupResult
	);

	HRESULT SaveOrUpdatePartBuild(
		int sessionId, 
		PartBuild pb, 
		int* SaveOrUpdatePartBuildResult
	);

	HRESULT DeleteDevelopLopById(
		int sessionId, 
		int id, 
		bool* DeleteDevelopLopByIdResult
	);

	HRESULT DownloadWorkLogById(
		int sessionId, 
		int id, 
		WorkLog* DownloadWorkLogByIdResult
	);

	HRESULT QueryAttendanceArrByUserId(
		int sessionId, 
		int userId, 
		BSTR startDate, 
		BSTR endDate, 
		Attendance** QueryAttendanceArrByUserIdResult, int* QueryAttendanceArrByUserIdResult_nSizeIs
	);

	HRESULT DeleteJobById(
		int sessionId, 
		int id, 
		bool* DeleteJobByIdResult
	);

	HRESULT UpdateTasksSerialNo(
		int sessionId, 
		int* idArr, int idArr_nSizeIs, 
		int* serialArr, int serialArr_nSizeIs, 
		bool* UpdateTasksSerialNoResult
	);

	HRESULT QueryAllUser(
		int sessionId, 
		User** QueryAllUserResult, int* QueryAllUserResult_nSizeIs
	);

	HRESULT canExecuteFuncOper(
		int sessionId, 
		int funcOperId, 
		bool* canExecuteFuncOperResult
	);

	HRESULT DeleteFeedBack(
		int sessionId, 
		int id, 
		bool* DeleteFeedBackResult
	);

	HRESULT DeletePartBuildById(
		int sessionId, 
		int id, 
		bool* DeletePartBuildByIdResult
	);

	HRESULT QueryTestItemByVersionId(
		int sessionId, 
		int versionId, 
		TestItem** QueryTestItemByVersionIdResult, int* QueryTestItemByVersionIdResult_nSizeIs
	);

	HRESULT QueryVersionUpdateInfoArr(
		BSTR production, 
		BSTR version, 
		VersionInfo** QueryVersionUpdateInfoArrResult, int* QueryVersionUpdateInfoArrResult_nSizeIs
	);

	HRESULT QueryCategoryArr(
		int sessionId, 
		Category** QueryCategoryArrResult, int* QueryCategoryArrResult_nSizeIs
	);

	HRESULT QueryFeedBacks(
		int sessionId, 
		int gradeId, 
		BSTR problemType, 
		BSTR processState, 
		int* productIdArr, int productIdArr_nSizeIs, 
		int pageNum, 
		int pageSize, 
		FeedBack** QueryFeedBacksResult, int* QueryFeedBacksResult_nSizeIs
	);

	HRESULT InserClientDownloadItem(
		int sessionId, 
		Client_DownloadItem cd, 
		bool* InserClientDownloadItemResult
	);

	HRESULT DeleteVersionRevision(
		int sessionId, 
		int id, 
		bool* DeleteVersionRevisionResult
	);

	HRESULT UpdateWorklog_testItem(
		int sessionId, 
		WorLog_testItem* worklogTestItemArr, int worklogTestItemArr_nSizeIs, 
		WorkLog wl, 
		bool* UpdateWorklog_testItemResult
	);

	HRESULT QueryTaskDispatchArrByReceiverId(
		int sessionId, 
		int userId, 
		bool finished, 
		bool unFinished, 
		BSTR startDate, 
		BSTR endDate, 
		TaskDispatch** QueryTaskDispatchArrByReceiverIdResult, int* QueryTaskDispatchArrByReceiverIdResult_nSizeIs
	);

	HRESULT DeleteNewsById(
		int sessionId, 
		int id, 
		bool* DeleteNewsByIdResult
	);

	HRESULT DownloadUser(
		int sessionId, 
		int id, 
		User* DownloadUserResult
	);

	HRESULT LoginUser(
		BSTR userName, 
		BSTR password, 
		LoginClass* LoginUserResult
	);

	HRESULT QueryAllPartBuild(
		PartBuild** QueryAllPartBuildResult, int* QueryAllPartBuildResult_nSizeIs
	);

	HRESULT QueryVersionTestItemArrByVersionId(
		int sessionId, 
		int versionId, 
		Version_testItem** QueryVersionTestItemArrByVersionIdResult, int* QueryVersionTestItemArrByVersionIdResult_nSizeIs
	);

	HRESULT GetFeedBack(
		FeedBack* GetFeedBackResult
	);

	HRESULT UpdateVersion_TestItem(
		int sessionId, 
		Version_testItem vt, 
		bool* UpdateVersion_TestItemResult
	);

	HRESULT DownloadProblemGrade(
		int sessionId, 
		int id, 
		ProblemGrade* DownloadProblemGradeResult
	);

	HRESULT SetSqlLink(
		BSTR userName, 
		BSTR password, 
		BSTR linkStr, 
		bool* SetSqlLinkResult
	);

	HRESULT QueryNewsArrByDate(
		int sessionId, 
		BSTR startDate, 
		BSTR endDate, 
		News** QueryNewsArrByDateResult, int* QueryNewsArrByDateResult_nSizeIs
	);
};

typedef CXMServerT<> CXMServer;

__if_not_exists(__Function_entries)
{
extern __declspec(selectany) const _soapmapentry __Function_entries[] =
{
	{ 
		0x3865A193, 
		"functionId", 
		L"functionId", 
		sizeof("functionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Function, functionId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x668BCF9F, 
		"functdesc", 
		L"functdesc", 
		sizeof("functdesc")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Function, functdesc),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Function_map =
{
	0xD0365226,
	"Function",
	L"Function",
	sizeof("Function")-1,
	sizeof("Function")-1,
	SOAPMAP_STRUCT,
	__Function_entries,
	sizeof(Function),
	2,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__RelationObject_entries)
{
extern __declspec(selectany) const _soapmapentry __RelationObject_entries[] =
{
	{ 
		0xAEA53BCB, 
		"relationId", 
		L"relationId", 
		sizeof("relationId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(RelationObject, relationId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xECE61A1F, 
		"relationName", 
		L"relationName", 
		sizeof("relationName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RelationObject, relationName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x834CCED9, 
		"relationObj", 
		L"relationObj", 
		sizeof("relationObj")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RelationObject, relationObj),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __RelationObject_map =
{
	0x593392F5,
	"RelationObject",
	L"RelationObject",
	sizeof("RelationObject")-1,
	sizeof("RelationObject")-1,
	SOAPMAP_STRUCT,
	__RelationObject_entries,
	sizeof(RelationObject),
	3,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__TestItem_entries)
{
extern __declspec(selectany) const _soapmapentry __TestItem_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(TestItem, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x085BB513, 
		"share", 
		L"share", 
		sizeof("share")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD, 
		offsetof(TestItem, share),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x086EAA02, 
		"title", 
		L"title", 
		sizeof("title")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TestItem, title),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA646FFA4, 
		"description", 
		L"description", 
		sizeof("description")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TestItem, description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x63013C1B, 
		"ProductFlag", 
		L"ProductFlag", 
		sizeof("ProductFlag")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(TestItem, ProductFlag),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1D24F6B9, 
		"categoryObj", 
		L"categoryObj", 
		sizeof("categoryObj")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(TestItem, categoryObj),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TestItem_map =
{
	0xF8F22F4F,
	"TestItem",
	L"TestItem",
	sizeof("TestItem")-1,
	sizeof("TestItem")-1,
	SOAPMAP_STRUCT,
	__TestItem_entries,
	sizeof(TestItem),
	6,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__DevelopLog_entries)
{
extern __declspec(selectany) const _soapmapentry __DevelopLog_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DevelopLog, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x086EAA02, 
		"title", 
		L"title", 
		sizeof("title")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DevelopLog, title),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA646FFA4, 
		"description", 
		L"description", 
		sizeof("description")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DevelopLog, description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9ED2666C, 
		"startDate", 
		L"startDate", 
		sizeof("startDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(DevelopLog, startDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x6689F575, 
		"endDate", 
		L"endDate", 
		sizeof("endDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(DevelopLog, endDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00421EFF, 
		"user", 
		L"user", 
		sizeof("user")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(DevelopLog, user),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x3E814EA7, 
		"production", 
		L"production", 
		sizeof("production")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(DevelopLog, production),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0xAE464D7D, 
		"relateFile", 
		L"relateFile", 
		sizeof("relateFile")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(DevelopLog, relateFile),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x713AECB4, 
		"testItemArr", 
		L"testItemArr", 
		sizeof("testItemArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(DevelopLog, testItemArr),
		NULL, 
		&__TestItem_map, 
		8+1 
	},
	{ 
		0x713AECB4, 
		"testItemArr", 
		L"testItemArr", 
		sizeof("testItemArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(DevelopLog, __testItemArr_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x0001FA5C, 
		"tag", 
		L"tag", 
		sizeof("tag")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DevelopLog, tag),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DevelopLog_map =
{
	0x50FDBA51,
	"DevelopLog",
	L"DevelopLog",
	sizeof("DevelopLog")-1,
	sizeof("DevelopLog")-1,
	SOAPMAP_STRUCT,
	__DevelopLog_entries,
	sizeof(DevelopLog),
	10,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__WorLog_testItem_entries)
{
extern __declspec(selectany) const _soapmapentry __WorLog_testItem_entries[] =
{
	{ 
		0x07E0F46F, 
		"logId", 
		L"logId", 
		sizeof("logId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(WorLog_testItem, logId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x15FB1AAD, 
		"testId", 
		L"testId", 
		sizeof("testId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(WorLog_testItem, testId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x7A95FBA8, 
		"processed", 
		L"processed", 
		sizeof("processed")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD, 
		offsetof(WorLog_testItem, processed),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __WorLog_testItem_map =
{
	0x3D7EF3E8,
	"WorLog_testItem",
	L"WorLog_testItem",
	sizeof("WorLog_testItem")-1,
	sizeof("WorLog_testItem")-1,
	SOAPMAP_STRUCT,
	__WorLog_testItem_entries,
	sizeof(WorLog_testItem),
	3,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__Attendance_entries)
{
extern __declspec(selectany) const _soapmapentry __Attendance_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Attendance, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00421EFF, 
		"user", 
		L"user", 
		sizeof("user")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(Attendance, user),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0xDBA4E0C2, 
		"supervisor", 
		L"supervisor", 
		sizeof("supervisor")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(Attendance, supervisor),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x42A5EDD2, 
		"submitDate", 
		L"submitDate", 
		sizeof("submitDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(Attendance, submitDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFA1792B5, 
		"attendanceDate", 
		L"attendanceDate", 
		sizeof("attendanceDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(Attendance, attendanceDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFA20BE59, 
		"attendanceType", 
		L"attendanceType", 
		sizeof("attendanceType")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Attendance, attendanceType),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x05A3FFFB, 
		"mender", 
		L"mender", 
		sizeof("mender")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Attendance, mender),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x08056069, 
		"notes", 
		L"notes", 
		sizeof("notes")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Attendance, notes),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Attendance_map =
{
	0x43809557,
	"Attendance",
	L"Attendance",
	sizeof("Attendance")-1,
	sizeof("Attendance")-1,
	SOAPMAP_STRUCT,
	__Attendance_entries,
	sizeof(Attendance),
	8,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__FileHeader_entries)
{
extern __declspec(selectany) const _soapmapentry __FileHeader_entries[] =
{
	{ 
		0xB81C15ED, 
		"filePath", 
		L"filePath", 
		sizeof("filePath")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(FileHeader, filePath),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(FileHeader, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB81EAD02, 
		"fileType", 
		L"fileType", 
		sizeof("fileType")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(FileHeader, fileType),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB81AFC41, 
		"fileName", 
		L"fileName", 
		sizeof("fileName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(FileHeader, fileName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8BF1C6B8, 
		"fileNameServer", 
		L"fileNameServer", 
		sizeof("fileNameServer")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(FileHeader, fileNameServer),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFAB2282B, 
		"hostId", 
		L"hostId", 
		sizeof("hostId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(FileHeader, hostId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x7E1612F8, 
		"ownerId", 
		L"ownerId", 
		sizeof("ownerId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(FileHeader, ownerId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x055F2C4D, 
		"matter", 
		L"matter", 
		sizeof("matter")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(FileHeader, matter),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA646FFA4, 
		"description", 
		L"description", 
		sizeof("description")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(FileHeader, description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0001FA5C, 
		"tag", 
		L"tag", 
		sizeof("tag")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(FileHeader, tag),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __FileHeader_map =
{
	0x9BBBCF49,
	"FileHeader",
	L"FileHeader",
	sizeof("FileHeader")-1,
	sizeof("FileHeader")-1,
	SOAPMAP_STRUCT,
	__FileHeader_entries,
	sizeof(FileHeader),
	10,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__Reply_entries)
{
extern __declspec(selectany) const _soapmapentry __Reply_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Reply, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x73C53E85, 
		"feedBack", 
		L"feedBack", 
		sizeof("feedBack")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(Reply, feedBack),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Reply, content),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCD550A9B, 
		"processMan", 
		L"processMan", 
		sizeof("processMan")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(Reply, processMan),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0xA4ADF127, 
		"attachmFileArr", 
		L"attachmFileArr", 
		sizeof("attachmFileArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(Reply, attachmFileArr),
		NULL, 
		&__FileHeader_map, 
		4+1 
	},
	{ 
		0xA4ADF127, 
		"attachmFileArr", 
		L"attachmFileArr", 
		sizeof("attachmFileArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(Reply, __attachmFileArr_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x77F16FBD, 
		"processDate", 
		L"processDate", 
		sizeof("processDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(Reply, processDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0001FA5C, 
		"tag", 
		L"tag", 
		sizeof("tag")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Reply, tag),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Reply_map =
{
	0x0605266C,
	"Reply",
	L"Reply",
	sizeof("Reply")-1,
	sizeof("Reply")-1,
	SOAPMAP_STRUCT,
	__Reply_entries,
	sizeof(Reply),
	7,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__Client_DownloadItem_entries)
{
extern __declspec(selectany) const _soapmapentry __Client_DownloadItem_entries[] =
{
	{ 
		0x3E293A6B, 
		"client_id", 
		L"client_id", 
		sizeof("client_id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Client_DownloadItem, client_id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xE0619313, 
		"downloadItem_id", 
		L"downloadItem_id", 
		sizeof("downloadItem_id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Client_DownloadItem, downloadItem_id),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Client_DownloadItem_map =
{
	0xF4E42425,
	"Client_DownloadItem",
	L"Client_DownloadItem",
	sizeof("Client_DownloadItem")-1,
	sizeof("Client_DownloadItem")-1,
	SOAPMAP_STRUCT,
	__Client_DownloadItem_entries,
	sizeof(Client_DownloadItem),
	2,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__Bug_entries)
{
extern __declspec(selectany) const _soapmapentry __Bug_entries[] =
{
	{ 
		0x072F49AB, 
		"bugId", 
		L"bugId", 
		sizeof("bugId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Bug, bugId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9025B5FD, 
		"bugDesc", 
		L"bugDesc", 
		sizeof("bugDesc")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Bug, bugDesc),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Bug_map =
{
	0x0001283E,
	"Bug",
	L"Bug",
	sizeof("Bug")-1,
	sizeof("Bug")-1,
	SOAPMAP_STRUCT,
	__Bug_entries,
	sizeof(Bug),
	2,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__VersionInfo_entries)
{
extern __declspec(selectany) const _soapmapentry __VersionInfo_entries[] =
{
	{ 
		0x42B02A33, 
		"versionId", 
		L"versionId", 
		sizeof("versionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(VersionInfo, versionId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x6F40CEE6, 
		"version", 
		L"version", 
		sizeof("version")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(VersionInfo, version),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x2672BAC8, 
		"productionName", 
		L"productionName", 
		sizeof("productionName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(VersionInfo, productionName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x92ECD15E, 
		"promulgateDate", 
		L"promulgateDate", 
		sizeof("promulgateDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(VersionInfo, promulgateDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x6D13D50F, 
		"addFunction", 
		L"addFunction", 
		sizeof("addFunction")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(VersionInfo, addFunction),
		NULL, 
		&__Function_map, 
		4+1 
	},
	{ 
		0x6D13D50F, 
		"addFunction", 
		L"addFunction", 
		sizeof("addFunction")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(VersionInfo, __addFunction_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0xE98DF587, 
		"addBug", 
		L"addBug", 
		sizeof("addBug")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(VersionInfo, addBug),
		NULL, 
		&__Bug_map, 
		6+1 
	},
	{ 
		0xE98DF587, 
		"addBug", 
		L"addBug", 
		sizeof("addBug")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(VersionInfo, __addBug_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __VersionInfo_map =
{
	0x03CD12F2,
	"VersionInfo",
	L"VersionInfo",
	sizeof("VersionInfo")-1,
	sizeof("VersionInfo")-1,
	SOAPMAP_STRUCT,
	__VersionInfo_entries,
	sizeof(VersionInfo),
	6,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__DownloadItem_entries)
{
extern __declspec(selectany) const _soapmapentry __DownloadItem_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DownloadItem, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xBCB51440, 
		"file_name", 
		L"file_name", 
		sizeof("file_name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DownloadItem, file_name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x086EAA02, 
		"title", 
		L"title", 
		sizeof("title")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DownloadItem, title),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x59AEFB21, 
		"updateDate", 
		L"updateDate", 
		sizeof("updateDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(DownloadItem, updateDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x08056069, 
		"notes", 
		L"notes", 
		sizeof("notes")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DownloadItem, notes),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0039BBA0, 
		"file", 
		L"file", 
		sizeof("file")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(DownloadItem, file),
		NULL, 
		&__FileHeader_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DownloadItem_map =
{
	0xB7FF8A47,
	"DownloadItem",
	L"DownloadItem",
	sizeof("DownloadItem")-1,
	sizeof("DownloadItem")-1,
	SOAPMAP_STRUCT,
	__DownloadItem_entries,
	sizeof(DownloadItem),
	6,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__TaskDispatch_entries)
{
extern __declspec(selectany) const _soapmapentry __TaskDispatch_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(TaskDispatch, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x5DF56FCF, 
		"acceptUser", 
		L"acceptUser", 
		sizeof("acceptUser")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(TaskDispatch, acceptUser),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x710A815C, 
		"promulgator", 
		L"promulgator", 
		sizeof("promulgator")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(TaskDispatch, promulgator),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x086EAA02, 
		"title", 
		L"title", 
		sizeof("title")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TaskDispatch, title),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TaskDispatch, content),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xE1755D74, 
		"taskState", 
		L"taskState", 
		sizeof("taskState")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(TaskDispatch, taskState),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xEE0C44EE, 
		"dispatchDate", 
		L"dispatchDate", 
		sizeof("dispatchDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(TaskDispatch, dispatchDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x59AEFB21, 
		"updateDate", 
		L"updateDate", 
		sizeof("updateDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(TaskDispatch, updateDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x2555DF01, 
		"relativeFileArr", 
		L"relativeFileArr", 
		sizeof("relativeFileArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(TaskDispatch, relativeFileArr),
		NULL, 
		&__FileHeader_map, 
		8+1 
	},
	{ 
		0x2555DF01, 
		"relativeFileArr", 
		L"relativeFileArr", 
		sizeof("relativeFileArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(TaskDispatch, __relativeFileArr_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x0848368C, 
		"reply", 
		L"reply", 
		sizeof("reply")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TaskDispatch, reply),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x08056069, 
		"notes", 
		L"notes", 
		sizeof("notes")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TaskDispatch, notes),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x114D33A2, 
		"remark", 
		L"remark", 
		sizeof("remark")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TaskDispatch, remark),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x7118E130, 
		"remarkVisible", 
		L"remarkVisible", 
		sizeof("remarkVisible")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD, 
		offsetof(TaskDispatch, remarkVisible),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA4296229, 
		"isGroup", 
		L"isGroup", 
		sizeof("isGroup")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD, 
		offsetof(TaskDispatch, isGroup),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0AB537DA, 
		"groupId", 
		L"groupId", 
		sizeof("groupId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(TaskDispatch, groupId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1827E7AB, 
		"categoryId", 
		L"categoryId", 
		sizeof("categoryId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(TaskDispatch, categoryId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0001FA5C, 
		"tag", 
		L"tag", 
		sizeof("tag")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TaskDispatch, tag),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x91DBC47D, 
		"serialNo", 
		L"serialNo", 
		sizeof("serialNo")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(TaskDispatch, serialNo),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0E97E43B, 
		"productFlag", 
		L"productFlag", 
		sizeof("productFlag")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(TaskDispatch, productFlag),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TaskDispatch_map =
{
	0x814059C3,
	"TaskDispatch",
	L"TaskDispatch",
	sizeof("TaskDispatch")-1,
	sizeof("TaskDispatch")-1,
	SOAPMAP_STRUCT,
	__TaskDispatch_entries,
	sizeof(TaskDispatch),
	19,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__Role_entries)
{
extern __declspec(selectany) const _soapmapentry __Role_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Role, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x990E6353, 
		"roleName", 
		L"roleName", 
		sizeof("roleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Role, roleName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x6433089D, 
		"funcOperPermBytes", 
		L"funcOperPermBytes", 
		sizeof("funcOperPermBytes")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Role, funcOperPermBytes),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x08056069, 
		"notes", 
		L"notes", 
		sizeof("notes")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Role, notes),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0001FA5C, 
		"tag", 
		L"tag", 
		sizeof("tag")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Role, tag),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Role_map =
{
	0x002EDD92,
	"Role",
	L"Role",
	sizeof("Role")-1,
	sizeof("Role")-1,
	SOAPMAP_STRUCT,
	__Role_entries,
	sizeof(Role),
	5,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__Production_entries)
{
extern __declspec(selectany) const _soapmapentry __Production_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Production, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1945E48C, 
		"userId", 
		L"userId", 
		sizeof("userId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Production, userId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x003DFCC1, 
		"name", 
		L"name", 
		sizeof("name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Production, name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x08056069, 
		"notes", 
		L"notes", 
		sizeof("notes")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Production, notes),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0001723C, 
		"Tag", 
		L"Tag", 
		sizeof("Tag")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Production, Tag),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Production_map =
{
	0xBD2F2A87,
	"Production",
	L"Production",
	sizeof("Production")-1,
	sizeof("Production")-1,
	SOAPMAP_STRUCT,
	__Production_entries,
	sizeof(Production),
	5,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__User_entries)
{
extern __declspec(selectany) const _soapmapentry __User_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(User, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xEECC5CBF, 
		"client", 
		L"client", 
		sizeof("client")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(User, client),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x8253F720, 
		"userName", 
		L"userName", 
		sizeof("userName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(User, userName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9C439233, 
		"password", 
		L"password", 
		sizeof("password")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(User, password),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC4F7AA4A, 
		"reallyName", 
		L"reallyName", 
		sizeof("reallyName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(User, reallyName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0001F6B0, 
		"sex", 
		L"sex", 
		sizeof("sex")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(User, sex),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00402E6C, 
		"rank", 
		L"rank", 
		sizeof("rank")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(User, rank),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x07611AC8, 
		"email", 
		L"email", 
		sizeof("email")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(User, email),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1F0BD867, 
		"nowTask", 
		L"nowTask", 
		sizeof("nowTask")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(User, nowTask),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x68833545, 
		"taskSubmitDate", 
		L"taskSubmitDate", 
		sizeof("taskSubmitDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(User, taskSubmitDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCB8FED2F, 
		"enrolTime", 
		L"enrolTime", 
		sizeof("enrolTime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(User, enrolTime),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x07432EA0, 
		"dWord", 
		L"dWord", 
		sizeof("dWord")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(User, dWord),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x08056069, 
		"notes", 
		L"notes", 
		sizeof("notes")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(User, notes),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x52368817, 
		"roleArr", 
		L"roleArr", 
		sizeof("roleArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(User, roleArr),
		NULL, 
		&__Role_map, 
		13+1 
	},
	{ 
		0x52368817, 
		"roleArr", 
		L"roleArr", 
		sizeof("roleArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(User, __roleArr_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x3E80A4E6, 
		"productArr", 
		L"productArr", 
		sizeof("productArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(User, productArr),
		NULL, 
		&__Production_map, 
		15+1 
	},
	{ 
		0x3E80A4E6, 
		"productArr", 
		L"productArr", 
		sizeof("productArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(User, __productArr_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x0001FA5C, 
		"tag", 
		L"tag", 
		sizeof("tag")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(User, tag),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __User_map =
{
	0x003092DF,
	"User",
	L"User",
	sizeof("User")-1,
	sizeof("User")-1,
	SOAPMAP_STRUCT,
	__User_entries,
	sizeof(User),
	16,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__Client_entries)
{
extern __declspec(selectany) const _soapmapentry __Client_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Client, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0AE99AE1, 
		"unitName", 
		L"unitName", 
		sizeof("unitName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Client, unitName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1B66B686, 
		"address", 
		L"address", 
		sizeof("address")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Client, address),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x76825C0A, 
		"linkman", 
		L"linkman", 
		sizeof("linkman")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Client, linkman),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x6BAC80C4, 
		"telephone", 
		L"telephone", 
		sizeof("telephone")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Client, telephone),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0001BEDF, 
		"fax", 
		L"fax", 
		sizeof("fax")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Client, fax),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xAFBB8E86, 
		"province", 
		L"province", 
		sizeof("province")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Client, province),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x339804AE, 
		"postalCode", 
		L"postalCode", 
		sizeof("postalCode")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Client, postalCode),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3F344B9B, 
		"enrollTime", 
		L"enrollTime", 
		sizeof("enrollTime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(Client, enrollTime),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x660344EC, 
		"productionArr", 
		L"productionArr", 
		sizeof("productionArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(Client, productionArr),
		NULL, 
		&__Production_map, 
		9+1 
	},
	{ 
		0x660344EC, 
		"productionArr", 
		L"productionArr", 
		sizeof("productionArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(Client, __productionArr_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0xE06114AC, 
		"downloadItemArr", 
		L"downloadItemArr", 
		sizeof("downloadItemArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(Client, downloadItemArr),
		NULL, 
		&__DownloadItem_map, 
		11+1 
	},
	{ 
		0xE06114AC, 
		"downloadItemArr", 
		L"downloadItemArr", 
		sizeof("downloadItemArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(Client, __downloadItemArr_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Client_map =
{
	0xA427489F,
	"Client",
	L"Client",
	sizeof("Client")-1,
	sizeof("Client")-1,
	SOAPMAP_STRUCT,
	__Client_entries,
	sizeof(Client),
	11,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__LoginClass_entries)
{
extern __declspec(selectany) const _soapmapentry __LoginClass_entries[] =
{
	{ 
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(LoginClass, sessionId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00421EFF, 
		"user", 
		L"user", 
		sizeof("user")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(LoginClass, user),
		NULL, 
		&__User_map, 
		-1 
	},
	{ 
		0x0001FA5C, 
		"tag", 
		L"tag", 
		sizeof("tag")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LoginClass, tag),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __LoginClass_map =
{
	0x5BC86AAF,
	"LoginClass",
	L"LoginClass",
	sizeof("LoginClass")-1,
	sizeof("LoginClass")-1,
	SOAPMAP_STRUCT,
	__LoginClass_entries,
	sizeof(LoginClass),
	3,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__PartBuild_entries)
{
extern __declspec(selectany) const _soapmapentry __PartBuild_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(PartBuild, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x003DFCC1, 
		"name", 
		L"name", 
		sizeof("name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(PartBuild, name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x08279B25, 
		"place", 
		L"place", 
		sizeof("place")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(PartBuild, place),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB12F8F93, 
		"partItemId", 
		L"partItemId", 
		sizeof("partItemId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(PartBuild, partItemId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x42A5EDD2, 
		"submitDate", 
		L"submitDate", 
		sizeof("submitDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(PartBuild, submitDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __PartBuild_map =
{
	0x46C6B707,
	"PartBuild",
	L"PartBuild",
	sizeof("PartBuild")-1,
	sizeof("PartBuild")-1,
	SOAPMAP_STRUCT,
	__PartBuild_entries,
	sizeof(PartBuild),
	5,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__TestItemSet_entries)
{
extern __declspec(selectany) const _soapmapentry __TestItemSet_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(TestItemSet, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x086EAA02, 
		"title", 
		L"title", 
		sizeof("title")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TestItemSet, title),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xBF658F25, 
		"proByte", 
		L"proByte", 
		sizeof("proByte")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TestItemSet, proByte),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1827E7AB, 
		"categoryId", 
		L"categoryId", 
		sizeof("categoryId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(TestItemSet, categoryId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x994D9451, 
		"submit_date", 
		L"submit_date", 
		sizeof("submit_date")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(TestItemSet, submit_date),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x08056069, 
		"notes", 
		L"notes", 
		sizeof("notes")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TestItemSet, notes),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TestItemSet_map =
{
	0xC5A48F7B,
	"TestItemSet",
	L"TestItemSet",
	sizeof("TestItemSet")-1,
	sizeof("TestItemSet")-1,
	SOAPMAP_STRUCT,
	__TestItemSet_entries,
	sizeof(TestItemSet),
	6,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__Category_entries)
{
extern __declspec(selectany) const _soapmapentry __Category_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Category, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x4CC82C1D, 
		"objType", 
		L"objType", 
		sizeof("objType")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Category, objType),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x003DFCC1, 
		"name", 
		L"name", 
		sizeof("name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Category, name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x98EC3317, 
		"parentId", 
		L"parentId", 
		sizeof("parentId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Category, parentId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xBF658F25, 
		"proByte", 
		L"proByte", 
		sizeof("proByte")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Category, proByte),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Category_map =
{
	0x1600323E,
	"Category",
	L"Category",
	sizeof("Category")-1,
	sizeof("Category")-1,
	SOAPMAP_STRUCT,
	__Category_entries,
	sizeof(Category),
	5,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__Client_Production_entries)
{
extern __declspec(selectany) const _soapmapentry __Client_Production_entries[] =
{
	{ 
		0x3E293A6B, 
		"client_id", 
		L"client_id", 
		sizeof("client_id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Client_Production, client_id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x6603C353, 
		"production_id", 
		L"production_id", 
		sizeof("production_id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Client_Production, production_id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0001FA5C, 
		"tag", 
		L"tag", 
		sizeof("tag")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Client_Production, tag),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Client_Production_map =
{
	0x71C7B4E5,
	"Client_Production",
	L"Client_Production",
	sizeof("Client_Production")-1,
	sizeof("Client_Production")-1,
	SOAPMAP_STRUCT,
	__Client_Production_entries,
	sizeof(Client_Production),
	3,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__XMSession_entries)
{
extern __declspec(selectany) const _soapmapentry __XMSession_entries[] =
{
	{ 
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(XMSession, sessionId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB2EEEFF7, 
		"RoleArr", 
		L"RoleArr", 
		sizeof("RoleArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(XMSession, RoleArr),
		NULL, 
		&__Role_map, 
		1+1 
	},
	{ 
		0xB2EEEFF7, 
		"RoleArr", 
		L"RoleArr", 
		sizeof("RoleArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(XMSession, __RoleArr_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __XMSession_map =
{
	0x03B07909,
	"XMSession",
	L"XMSession",
	sizeof("XMSession")-1,
	sizeof("XMSession")-1,
	SOAPMAP_STRUCT,
	__XMSession_entries,
	sizeof(XMSession),
	2,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__LoginStruct_entries)
{
extern __declspec(selectany) const _soapmapentry __LoginStruct_entries[] =
{
	{ 
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(LoginStruct, sessionId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00421EFF, 
		"user", 
		L"user", 
		sizeof("user")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(LoginStruct, user),
		NULL, 
		&__User_map, 
		-1 
	},
	{ 
		0x00018098, 
		"XMS", 
		L"XMS", 
		sizeof("XMS")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(LoginStruct, XMS),
		NULL, 
		&__XMSession_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __LoginStruct_map =
{
	0xFAC267FE,
	"LoginStruct",
	L"LoginStruct",
	sizeof("LoginStruct")-1,
	sizeof("LoginStruct")-1,
	SOAPMAP_STRUCT,
	__LoginStruct_entries,
	sizeof(LoginStruct),
	3,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__WorkLog_entries)
{
extern __declspec(selectany) const _soapmapentry __WorkLog_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(WorkLog, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x028392AB, 
		"hasTestItem", 
		L"hasTestItem", 
		sizeof("hasTestItem")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD, 
		offsetof(WorkLog, hasTestItem),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA646FFA4, 
		"description", 
		L"description", 
		sizeof("description")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(WorkLog, description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x42A5EDD2, 
		"submitDate", 
		L"submitDate", 
		sizeof("submitDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(WorkLog, submitDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00421EFF, 
		"user", 
		L"user", 
		sizeof("user")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(WorkLog, user),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x086EAA02, 
		"title", 
		L"title", 
		sizeof("title")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(WorkLog, title),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x08056069, 
		"notes", 
		L"notes", 
		sizeof("notes")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(WorkLog, notes),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9ED2666C, 
		"startDate", 
		L"startDate", 
		sizeof("startDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(WorkLog, startDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x6689F575, 
		"endDate", 
		L"endDate", 
		sizeof("endDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(WorkLog, endDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x713AECB4, 
		"testItemArr", 
		L"testItemArr", 
		sizeof("testItemArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(WorkLog, testItemArr),
		NULL, 
		&__TestItem_map, 
		9+1 
	},
	{ 
		0x713AECB4, 
		"testItemArr", 
		L"testItemArr", 
		sizeof("testItemArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(WorkLog, __testItemArr_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __WorkLog_map =
{
	0x34422B65,
	"WorkLog",
	L"WorkLog",
	sizeof("WorkLog")-1,
	sizeof("WorkLog")-1,
	SOAPMAP_STRUCT,
	__WorkLog_entries,
	sizeof(WorkLog),
	10,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__Version_testItem_entries)
{
extern __declspec(selectany) const _soapmapentry __Version_testItem_entries[] =
{
	{ 
		0x6F40CEE6, 
		"version", 
		L"version", 
		sizeof("version")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(Version_testItem, version),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x812CCB6F, 
		"testItem", 
		L"testItem", 
		sizeof("testItem")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(Version_testItem, testItem),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x7A95FBA8, 
		"processed", 
		L"processed", 
		sizeof("processed")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD, 
		offsetof(Version_testItem, processed),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x08056069, 
		"notes", 
		L"notes", 
		sizeof("notes")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Version_testItem, notes),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1150D23F, 
		"result", 
		L"result", 
		sizeof("result")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD, 
		offsetof(Version_testItem, result),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00421EFF, 
		"user", 
		L"user", 
		sizeof("user")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(Version_testItem, user),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x91784B80, 
		"update_date", 
		L"update_date", 
		sizeof("update_date")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(Version_testItem, update_date),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Version_testItem_map =
{
	0xEE3CF054,
	"Version_testItem",
	L"Version_testItem",
	sizeof("Version_testItem")-1,
	sizeof("Version_testItem")-1,
	SOAPMAP_STRUCT,
	__Version_testItem_entries,
	sizeof(Version_testItem),
	7,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__Job_entries)
{
extern __declspec(selectany) const _soapmapentry __Job_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Job, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x086EAA02, 
		"title", 
		L"title", 
		sizeof("title")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Job, title),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Job, content),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x710A815C, 
		"promulgator", 
		L"promulgator", 
		sizeof("promulgator")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(Job, promulgator),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x003881DE, 
		"date", 
		L"date", 
		sizeof("date")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(Job, date),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Job_map =
{
	0x0001497B,
	"Job",
	L"Job",
	sizeof("Job")-1,
	sizeof("Job")-1,
	SOAPMAP_STRUCT,
	__Job_entries,
	sizeof(Job),
	5,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__VersionRevision_entries)
{
extern __declspec(selectany) const _soapmapentry __VersionRevision_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(VersionRevision, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x6F40CEE6, 
		"version", 
		L"version", 
		sizeof("version")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(VersionRevision, version),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xBF7820E1, 
		"product", 
		L"product", 
		sizeof("product")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(VersionRevision, product),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x92ECD15E, 
		"promulgateDate", 
		L"promulgateDate", 
		sizeof("promulgateDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(VersionRevision, promulgateDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x710A815C, 
		"promulgator", 
		L"promulgator", 
		sizeof("promulgator")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(VersionRevision, promulgator),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x2555DF01, 
		"relativeFileArr", 
		L"relativeFileArr", 
		sizeof("relativeFileArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(VersionRevision, relativeFileArr),
		NULL, 
		&__FileHeader_map, 
		5+1 
	},
	{ 
		0x2555DF01, 
		"relativeFileArr", 
		L"relativeFileArr", 
		sizeof("relativeFileArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(VersionRevision, __relativeFileArr_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0xD9739600, 
		"descriptionInner", 
		L"descriptionInner", 
		sizeof("descriptionInner")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(VersionRevision, descriptionInner),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xD4B5ECBC, 
		"descriptionOut", 
		L"descriptionOut", 
		sizeof("descriptionOut")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(VersionRevision, descriptionOut),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0001FA5C, 
		"tag", 
		L"tag", 
		sizeof("tag")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(VersionRevision, tag),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __VersionRevision_map =
{
	0x37CC6915,
	"VersionRevision",
	L"VersionRevision",
	sizeof("VersionRevision")-1,
	sizeof("VersionRevision")-1,
	SOAPMAP_STRUCT,
	__VersionRevision_entries,
	sizeof(VersionRevision),
	9,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__News_entries)
{
extern __declspec(selectany) const _soapmapentry __News_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(News, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x086EAA02, 
		"title", 
		L"title", 
		sizeof("title")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(News, title),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(News, content),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x953C7767, 
		"issueDate", 
		L"issueDate", 
		sizeof("issueDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(News, issueDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x710A815C, 
		"promulgator", 
		L"promulgator", 
		sizeof("promulgator")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(News, promulgator),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __News_map =
{
	0x002C82FD,
	"News",
	L"News",
	sizeof("News")-1,
	sizeof("News")-1,
	SOAPMAP_STRUCT,
	__News_entries,
	sizeof(News),
	5,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__Notification_entries)
{
extern __declspec(selectany) const _soapmapentry __Notification_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Notification, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x16A055CB, 
		"receiverFlagWord", 
		L"receiverFlagWord", 
		sizeof("receiverFlagWord")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Notification, receiverFlagWord),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x086EAA02, 
		"title", 
		L"title", 
		sizeof("title")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Notification, title),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Notification, content),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x92ECD15E, 
		"promulgateDate", 
		L"promulgateDate", 
		sizeof("promulgateDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(Notification, promulgateDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x710A815C, 
		"promulgator", 
		L"promulgator", 
		sizeof("promulgator")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(Notification, promulgator),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x2555DF01, 
		"relativeFileArr", 
		L"relativeFileArr", 
		sizeof("relativeFileArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(Notification, relativeFileArr),
		NULL, 
		&__FileHeader_map, 
		6+1 
	},
	{ 
		0x2555DF01, 
		"relativeFileArr", 
		L"relativeFileArr", 
		sizeof("relativeFileArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(Notification, __relativeFileArr_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x08056069, 
		"notes", 
		L"notes", 
		sizeof("notes")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Notification, notes),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0001FA5C, 
		"tag", 
		L"tag", 
		sizeof("tag")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Notification, tag),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Notification_map =
{
	0xFC164C67,
	"Notification",
	L"Notification",
	sizeof("Notification")-1,
	sizeof("Notification")-1,
	SOAPMAP_STRUCT,
	__Notification_entries,
	sizeof(Notification),
	9,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__ProblemGrade_entries)
{
extern __declspec(selectany) const _soapmapentry __ProblemGrade_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(ProblemGrade, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x2EC74324, 
		"gradeName", 
		L"gradeName", 
		sizeof("gradeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ProblemGrade, gradeName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x08056069, 
		"notes", 
		L"notes", 
		sizeof("notes")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ProblemGrade, notes),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ProblemGrade_map =
{
	0x15952174,
	"ProblemGrade",
	L"ProblemGrade",
	sizeof("ProblemGrade")-1,
	sizeof("ProblemGrade")-1,
	SOAPMAP_STRUCT,
	__ProblemGrade_entries,
	sizeof(ProblemGrade),
	3,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__FeedBack_entries)
{
extern __declspec(selectany) const _soapmapentry __FeedBack_entries[] =
{
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(FeedBack, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x086EAA02, 
		"title", 
		L"title", 
		sizeof("title")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(FeedBack, title),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x6F40CEE6, 
		"version", 
		L"version", 
		sizeof("version")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(FeedBack, version),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(FeedBack, content),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8270F6D5, 
		"scanNum", 
		L"scanNum", 
		sizeof("scanNum")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(FeedBack, scanNum),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3E814EA7, 
		"production", 
		L"production", 
		sizeof("production")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(FeedBack, production),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x3400CD94, 
		"problemGrade", 
		L"problemGrade", 
		sizeof("problemGrade")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(FeedBack, problemGrade),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0xAF525B5A, 
		"proposer", 
		L"proposer", 
		sizeof("proposer")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(FeedBack, proposer),
		NULL, 
		&__RelationObject_map, 
		-1 
	},
	{ 
		0x42A5EDD2, 
		"submitDate", 
		L"submitDate", 
		sizeof("submitDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(FeedBack, submitDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x59AEFB21, 
		"updateDate", 
		L"updateDate", 
		sizeof("updateDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD, 
		offsetof(FeedBack, updateDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x92ECE104, 
		"isEssential", 
		L"isEssential", 
		sizeof("isEssential")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD, 
		offsetof(FeedBack, isEssential),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x078846CD, 
		"group", 
		L"group", 
		sizeof("group")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD, 
		offsetof(FeedBack, group),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xDABCE346, 
		"superiorId", 
		L"superiorId", 
		sizeof("superiorId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(FeedBack, superiorId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x086249E1, 
		"state", 
		L"state", 
		sizeof("state")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(FeedBack, state),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x858D3013, 
		"problemtype", 
		L"problemtype", 
		sizeof("problemtype")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(FeedBack, problemtype),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x2555DF01, 
		"relativeFileArr", 
		L"relativeFileArr", 
		sizeof("relativeFileArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(FeedBack, relativeFileArr),
		NULL, 
		&__FileHeader_map, 
		15+1 
	},
	{ 
		0x2555DF01, 
		"relativeFileArr", 
		L"relativeFileArr", 
		sizeof("relativeFileArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(FeedBack, __relativeFileArr_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0xA1325EB1, 
		"replyArr", 
		L"replyArr", 
		sizeof("replyArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(FeedBack, replyArr),
		NULL, 
		&__Reply_map, 
		17+1 
	},
	{ 
		0xA1325EB1, 
		"replyArr", 
		L"replyArr", 
		sizeof("replyArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(FeedBack, __replyArr_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x91DBC47D, 
		"serialNo", 
		L"serialNo", 
		sizeof("serialNo")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(FeedBack, serialNo),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0001FA5C, 
		"tag", 
		L"tag", 
		sizeof("tag")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(FeedBack, tag),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xD662A44C, 
		"shadowSuperiorId", 
		L"shadowSuperiorId", 
		sizeof("shadowSuperiorId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(FeedBack, shadowSuperiorId),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __FeedBack_map =
{
	0xEB8AA265,
	"FeedBack",
	L"FeedBack",
	sizeof("FeedBack")-1,
	sizeof("FeedBack")-1,
	SOAPMAP_STRUCT,
	__FeedBack_entries,
	sizeof(FeedBack),
	20,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

struct __CXMServer_GetUser_struct
{
	User GetUserResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_GetUser_entries[] =
{

	{
		0xB0DE57BE, 
		"GetUserResult", 
		L"GetUserResult", 
		sizeof("GetUserResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_GetUser_struct, GetUserResult),
		NULL,
		&__User_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_GetUser_map =
{
	0x617A5F8E,
	"GetUser",
	L"GetUserResponse",
	sizeof("GetUser")-1,
	sizeof("GetUserResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_GetUser_entries,
	sizeof(__CXMServer_GetUser_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownloadNotification_struct
{
	int sessionId;
	int id;
	Notification DownloadNotificationResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownloadNotification_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadNotification_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadNotification_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x8CC6257E, 
		"DownloadNotificationResult", 
		L"DownloadNotificationResult", 
		sizeof("DownloadNotificationResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadNotification_struct, DownloadNotificationResult),
		NULL,
		&__Notification_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownloadNotification_map =
{
	0xD68C9D4E,
	"DownloadNotification",
	L"DownloadNotificationResponse",
	sizeof("DownloadNotification")-1,
	sizeof("DownloadNotificationResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownloadNotification_entries,
	sizeof(__CXMServer_DownloadNotification_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateTaskDispatch_struct
{
	int sessionId;
	TaskDispatch taskDispatch;
	bool isFile;
	int SaveOrUpdateTaskDispatchResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateTaskDispatch_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateTaskDispatch_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x9FAC05E3, 
		"taskDispatch", 
		L"taskDispatch", 
		sizeof("taskDispatch")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateTaskDispatch_struct, taskDispatch),
		NULL,
		&__TaskDispatch_map,
		-1,
	},
	{
		0xFD36DB7C, 
		"isFile", 
		L"isFile", 
		sizeof("isFile")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateTaskDispatch_struct, isFile),
		NULL,
		NULL,
		-1,
	},
	{
		0xEC498AB5, 
		"SaveOrUpdateTaskDispatchResult", 
		L"SaveOrUpdateTaskDispatchResult", 
		sizeof("SaveOrUpdateTaskDispatchResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateTaskDispatch_struct, SaveOrUpdateTaskDispatchResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateTaskDispatch_map =
{
	0x247E2C45,
	"SaveOrUpdateTaskDispatch",
	L"SaveOrUpdateTaskDispatchResponse",
	sizeof("SaveOrUpdateTaskDispatch")-1,
	sizeof("SaveOrUpdateTaskDispatchResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateTaskDispatch_entries,
	sizeof(__CXMServer_SaveOrUpdateTaskDispatch_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateDownloadItem_struct
{
	int sessionId;
	DownloadItem item;
	int SaveOrUpdateDownloadItemResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateDownloadItem_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateDownloadItem_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x003B8EAF, 
		"item", 
		L"item", 
		sizeof("item")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateDownloadItem_struct, item),
		NULL,
		&__DownloadItem_map,
		-1,
	},
	{
		0x92B40E39, 
		"SaveOrUpdateDownloadItemResult", 
		L"SaveOrUpdateDownloadItemResult", 
		sizeof("SaveOrUpdateDownloadItemResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateDownloadItem_struct, SaveOrUpdateDownloadItemResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateDownloadItem_map =
{
	0x0F97A0C9,
	"SaveOrUpdateDownloadItem",
	L"SaveOrUpdateDownloadItemResponse",
	sizeof("SaveOrUpdateDownloadItem")-1,
	sizeof("SaveOrUpdateDownloadItemResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateDownloadItem_entries,
	sizeof(__CXMServer_SaveOrUpdateDownloadItem_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryFeedBackArrByProblemType_struct
{
	int sessionId;
	int problemType;
	FeedBack *QueryFeedBackArrByProblemTypeResult;
	int __QueryFeedBackArrByProblemTypeResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryFeedBackArrByProblemType_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrByProblemType_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x857BA3F3, 
		"problemType", 
		L"problemType", 
		sizeof("problemType")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrByProblemType_struct, problemType),
		NULL,
		NULL,
		-1,
	},
	{
		0x48087EAD, 
		"QueryFeedBackArrByProblemTypeResult", 
		L"QueryFeedBackArrByProblemTypeResult", 
		sizeof("QueryFeedBackArrByProblemTypeResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBackArrByProblemType_struct, QueryFeedBackArrByProblemTypeResult),
		NULL,
		&__FeedBack_map,
		2+1,
	},
	{
		0x48087EAD,
		"__QueryFeedBackArrByProblemTypeResult_nSizeIs",
		L"__QueryFeedBackArrByProblemTypeResult_nSizeIs",
		sizeof("__QueryFeedBackArrByProblemTypeResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryFeedBackArrByProblemType_struct, __QueryFeedBackArrByProblemTypeResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryFeedBackArrByProblemType_map =
{
	0x6BC9FE3D,
	"QueryFeedBackArrByProblemType",
	L"QueryFeedBackArrByProblemTypeResponse",
	sizeof("QueryFeedBackArrByProblemType")-1,
	sizeof("QueryFeedBackArrByProblemTypeResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryFeedBackArrByProblemType_entries,
	sizeof(__CXMServer_QueryFeedBackArrByProblemType_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryProblemGradeArr_struct
{
	int sessionId;
	ProblemGrade *QueryProblemGradeArrResult;
	int __QueryProblemGradeArrResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryProblemGradeArr_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryProblemGradeArr_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xC43CC92E, 
		"QueryProblemGradeArrResult", 
		L"QueryProblemGradeArrResult", 
		sizeof("QueryProblemGradeArrResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryProblemGradeArr_struct, QueryProblemGradeArrResult),
		NULL,
		&__ProblemGrade_map,
		1+1,
	},
	{
		0xC43CC92E,
		"__QueryProblemGradeArrResult_nSizeIs",
		L"__QueryProblemGradeArrResult_nSizeIs",
		sizeof("__QueryProblemGradeArrResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryProblemGradeArr_struct, __QueryProblemGradeArrResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryProblemGradeArr_map =
{
	0xC63AECFE,
	"QueryProblemGradeArr",
	L"QueryProblemGradeArrResponse",
	sizeof("QueryProblemGradeArr")-1,
	sizeof("QueryProblemGradeArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryProblemGradeArr_entries,
	sizeof(__CXMServer_QueryProblemGradeArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_UpdateUser_struct
{
	int sessionId;
	User user;
	bool isRole;
	bool isProduct;
	bool UpdateUserResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_UpdateUser_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateUser_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00421EFF, 
		"user", 
		L"user", 
		sizeof("user")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateUser_struct, user),
		NULL,
		&__User_map,
		-1,
	},
	{
		0xFD3D898E, 
		"isRole", 
		L"isRole", 
		sizeof("isRole")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateUser_struct, isRole),
		NULL,
		NULL,
		-1,
	},
	{
		0x08CD833D, 
		"isProduct", 
		L"isProduct", 
		sizeof("isProduct")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateUser_struct, isProduct),
		NULL,
		NULL,
		-1,
	},
	{
		0x705543C1, 
		"UpdateUserResult", 
		L"UpdateUserResult", 
		sizeof("UpdateUserResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateUser_struct, UpdateUserResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_UpdateUser_map =
{
	0xDA5C5851,
	"UpdateUser",
	L"UpdateUserResponse",
	sizeof("UpdateUser")-1,
	sizeof("UpdateUserResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_UpdateUser_entries,
	sizeof(__CXMServer_UpdateUser_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownLoadNews_struct
{
	int sessionId;
	int id;
	News DownLoadNewsResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownLoadNews_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadNews_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadNews_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xC3730874, 
		"DownLoadNewsResult", 
		L"DownLoadNewsResult", 
		sizeof("DownLoadNewsResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadNews_struct, DownLoadNewsResult),
		NULL,
		&__News_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownLoadNews_map =
{
	0x6BFE15C4,
	"DownLoadNews",
	L"DownLoadNewsResponse",
	sizeof("DownLoadNews")-1,
	sizeof("DownLoadNewsResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownLoadNews_entries,
	sizeof(__CXMServer_DownLoadNews_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryProductionArr_struct
{
	int sessionId;
	Production *QueryProductionArrResult;
	int __QueryProductionArrResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryProductionArr_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryProductionArr_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x831B5221, 
		"QueryProductionArrResult", 
		L"QueryProductionArrResult", 
		sizeof("QueryProductionArrResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryProductionArr_struct, QueryProductionArrResult),
		NULL,
		&__Production_map,
		1+1,
	},
	{
		0x831B5221,
		"__QueryProductionArrResult_nSizeIs",
		L"__QueryProductionArrResult_nSizeIs",
		sizeof("__QueryProductionArrResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryProductionArr_struct, __QueryProductionArrResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryProductionArr_map =
{
	0xB6DF7EB1,
	"QueryProductionArr",
	L"QueryProductionArrResponse",
	sizeof("QueryProductionArr")-1,
	sizeof("QueryProductionArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryProductionArr_entries,
	sizeof(__CXMServer_QueryProductionArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryAllRole_struct
{
	int sessionId;
	Role *QueryAllRoleResult;
	int __QueryAllRoleResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryAllRole_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryAllRole_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xAA236D00, 
		"QueryAllRoleResult", 
		L"QueryAllRoleResult", 
		sizeof("QueryAllRoleResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryAllRole_struct, QueryAllRoleResult),
		NULL,
		&__Role_map,
		1+1,
	},
	{
		0xAA236D00,
		"__QueryAllRoleResult_nSizeIs",
		L"__QueryAllRoleResult_nSizeIs",
		sizeof("__QueryAllRoleResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryAllRole_struct, __QueryAllRoleResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryAllRole_map =
{
	0xC059CD50,
	"QueryAllRole",
	L"QueryAllRoleResponse",
	sizeof("QueryAllRole")-1,
	sizeof("QueryAllRoleResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryAllRole_entries,
	sizeof(__CXMServer_QueryAllRole_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_InsertUser_struct
{
	int sessionId;
	User newUser;
	int InsertUserResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_InsertUser_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_InsertUser_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x07B91949, 
		"newUser", 
		L"newUser", 
		sizeof("newUser")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_InsertUser_struct, newUser),
		NULL,
		&__User_map,
		-1,
	},
	{
		0xBC100C33, 
		"InsertUserResult", 
		L"InsertUserResult", 
		sizeof("InsertUserResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_InsertUser_struct, InsertUserResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_InsertUser_map =
{
	0xFFEB0543,
	"InsertUser",
	L"InsertUserResponse",
	sizeof("InsertUser")-1,
	sizeof("InsertUserResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_InsertUser_entries,
	sizeof(__CXMServer_InsertUser_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_UpdateUserTask_struct
{
	int sessionId;
	int userId;
	BSTR nowTask;
	bool UpdateUserTaskResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_UpdateUserTask_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateUserTask_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x1945E48C, 
		"userId", 
		L"userId", 
		sizeof("userId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateUserTask_struct, userId),
		NULL,
		NULL,
		-1,
	},
	{
		0x1F0BD867, 
		"nowTask", 
		L"nowTask", 
		sizeof("nowTask")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_UpdateUserTask_struct, nowTask),
		NULL,
		NULL,
		-1,
	},
	{
		0x53CBBAB4, 
		"UpdateUserTaskResult", 
		L"UpdateUserTaskResult", 
		sizeof("UpdateUserTaskResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateUserTask_struct, UpdateUserTaskResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_UpdateUserTask_map =
{
	0x754C5804,
	"UpdateUserTask",
	L"UpdateUserTaskResponse",
	sizeof("UpdateUserTask")-1,
	sizeof("UpdateUserTaskResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_UpdateUserTask_entries,
	sizeof(__CXMServer_UpdateUserTask_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryChildTaskDispatchsByGroupId_struct
{
	int sessionId;
	int groupId;
	TaskDispatch *QueryChildTaskDispatchsByGroupIdResult;
	int __QueryChildTaskDispatchsByGroupIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryChildTaskDispatchsByGroupId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryChildTaskDispatchsByGroupId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x0AB537DA, 
		"groupId", 
		L"groupId", 
		sizeof("groupId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryChildTaskDispatchsByGroupId_struct, groupId),
		NULL,
		NULL,
		-1,
	},
	{
		0x83E6E6E4, 
		"QueryChildTaskDispatchsByGroupIdResult", 
		L"QueryChildTaskDispatchsByGroupIdResult", 
		sizeof("QueryChildTaskDispatchsByGroupIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryChildTaskDispatchsByGroupId_struct, QueryChildTaskDispatchsByGroupIdResult),
		NULL,
		&__TaskDispatch_map,
		2+1,
	},
	{
		0x83E6E6E4,
		"__QueryChildTaskDispatchsByGroupIdResult_nSizeIs",
		L"__QueryChildTaskDispatchsByGroupIdResult_nSizeIs",
		sizeof("__QueryChildTaskDispatchsByGroupIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryChildTaskDispatchsByGroupId_struct, __QueryChildTaskDispatchsByGroupIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryChildTaskDispatchsByGroupId_map =
{
	0x18E35034,
	"QueryChildTaskDispatchsByGroupId",
	L"QueryChildTaskDispatchsByGroupIdResponse",
	sizeof("QueryChildTaskDispatchsByGroupId")-1,
	sizeof("QueryChildTaskDispatchsByGroupIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryChildTaskDispatchsByGroupId_entries,
	sizeof(__CXMServer_QueryChildTaskDispatchsByGroupId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_UpdateTestItemArr_struct
{
	int sessionId;
	TestItem *tiArr;
	int __tiArr_nSizeIs;
	bool UpdateTestItemArrResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_UpdateTestItemArr_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateTestItemArr_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x086DD1E2, 
		"tiArr", 
		L"tiArr", 
		sizeof("tiArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_UpdateTestItemArr_struct, tiArr),
		NULL,
		&__TestItem_map,
		1+1,
	},
	{
		0x086DD1E2,
		"__tiArr_nSizeIs",
		L"__tiArr_nSizeIs",
		sizeof("__tiArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_UpdateTestItemArr_struct, __tiArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xEAE8B7D6, 
		"UpdateTestItemArrResult", 
		L"UpdateTestItemArrResult", 
		sizeof("UpdateTestItemArrResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateTestItemArr_struct, UpdateTestItemArrResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_UpdateTestItemArr_map =
{
	0x479D25A6,
	"UpdateTestItemArr",
	L"UpdateTestItemArrResponse",
	sizeof("UpdateTestItemArr")-1,
	sizeof("UpdateTestItemArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_UpdateTestItemArr_entries,
	sizeof(__CXMServer_UpdateTestItemArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_GetVersionRevision_struct
{
	VersionRevision GetVersionRevisionResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_GetVersionRevision_entries[] =
{

	{
		0xF0E34E74, 
		"GetVersionRevisionResult", 
		L"GetVersionRevisionResult", 
		sizeof("GetVersionRevisionResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_GetVersionRevision_struct, GetVersionRevisionResult),
		NULL,
		&__VersionRevision_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_GetVersionRevision_map =
{
	0xB697DBC4,
	"GetVersionRevision",
	L"GetVersionRevisionResponse",
	sizeof("GetVersionRevision")-1,
	sizeof("GetVersionRevisionResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_GetVersionRevision_entries,
	sizeof(__CXMServer_GetVersionRevision_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteCategoryById_struct
{
	int sessionId;
	int id;
	bool DeleteCategoryByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteCategoryById_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteCategoryById_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteCategoryById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xE47B7BB8, 
		"DeleteCategoryByIdResult", 
		L"DeleteCategoryByIdResult", 
		sizeof("DeleteCategoryByIdResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteCategoryById_struct, DeleteCategoryByIdResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteCategoryById_map =
{
	0xF0F06A08,
	"DeleteCategoryById",
	L"DeleteCategoryByIdResponse",
	sizeof("DeleteCategoryById")-1,
	sizeof("DeleteCategoryByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteCategoryById_entries,
	sizeof(__CXMServer_DeleteCategoryById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryVersionArr_struct
{
	int sessionId;
	int productionId;
	BSTR startDate;
	BSTR endDate;
	BSTR keyword;
	VersionRevision *QueryVersionArrResult;
	int __QueryVersionArrResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryVersionArr_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryVersionArr_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xE40F9E34, 
		"productionId", 
		L"productionId", 
		sizeof("productionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryVersionArr_struct, productionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x9ED2666C, 
		"startDate", 
		L"startDate", 
		sizeof("startDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryVersionArr_struct, startDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x6689F575, 
		"endDate", 
		L"endDate", 
		sizeof("endDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryVersionArr_struct, endDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x21012985, 
		"keyword", 
		L"keyword", 
		sizeof("keyword")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryVersionArr_struct, keyword),
		NULL,
		NULL,
		-1,
	},
	{
		0x31EB5100, 
		"QueryVersionArrResult", 
		L"QueryVersionArrResult", 
		sizeof("QueryVersionArrResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryVersionArr_struct, QueryVersionArrResult),
		NULL,
		&__VersionRevision_map,
		5+1,
	},
	{
		0x31EB5100,
		"__QueryVersionArrResult_nSizeIs",
		L"__QueryVersionArrResult_nSizeIs",
		sizeof("__QueryVersionArrResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryVersionArr_struct, __QueryVersionArrResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryVersionArr_map =
{
	0x59AAB150,
	"QueryVersionArr",
	L"QueryVersionArrResponse",
	sizeof("QueryVersionArr")-1,
	sizeof("QueryVersionArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryVersionArr_entries,
	sizeof(__CXMServer_QueryVersionArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryTestItemSetArrByProId_struct
{
	int sessionId;
	int proId;
	TestItemSet *QueryTestItemSetArrByProIdResult;
	int __QueryTestItemSetArrByProIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryTestItemSetArrByProId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemSetArrByProId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x082B1D9E, 
		"proId", 
		L"proId", 
		sizeof("proId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemSetArrByProId_struct, proId),
		NULL,
		NULL,
		-1,
	},
	{
		0x267041AE, 
		"QueryTestItemSetArrByProIdResult", 
		L"QueryTestItemSetArrByProIdResult", 
		sizeof("QueryTestItemSetArrByProIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryTestItemSetArrByProId_struct, QueryTestItemSetArrByProIdResult),
		NULL,
		&__TestItemSet_map,
		2+1,
	},
	{
		0x267041AE,
		"__QueryTestItemSetArrByProIdResult_nSizeIs",
		L"__QueryTestItemSetArrByProIdResult_nSizeIs",
		sizeof("__QueryTestItemSetArrByProIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryTestItemSetArrByProId_struct, __QueryTestItemSetArrByProIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryTestItemSetArrByProId_map =
{
	0x832E857E,
	"QueryTestItemSetArrByProId",
	L"QueryTestItemSetArrByProIdResponse",
	sizeof("QueryTestItemSetArrByProId")-1,
	sizeof("QueryTestItemSetArrByProIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryTestItemSetArrByProId_entries,
	sizeof(__CXMServer_QueryTestItemSetArrByProId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_GetFileHeader_struct
{
	FileHeader GetFileHeaderResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_GetFileHeader_entries[] =
{

	{
		0xD32A5BA8, 
		"GetFileHeaderResult", 
		L"GetFileHeaderResult", 
		sizeof("GetFileHeaderResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_GetFileHeader_struct, GetFileHeaderResult),
		NULL,
		&__FileHeader_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_GetFileHeader_map =
{
	0x46D705F8,
	"GetFileHeader",
	L"GetFileHeaderResponse",
	sizeof("GetFileHeader")-1,
	sizeof("GetFileHeaderResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_GetFileHeader_entries,
	sizeof(__CXMServer_GetFileHeader_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_GetReply_struct
{
	Reply GetReplyResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_GetReply_entries[] =
{

	{
		0x089C110B, 
		"GetReplyResult", 
		L"GetReplyResult", 
		sizeof("GetReplyResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_GetReply_struct, GetReplyResult),
		NULL,
		&__Reply_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_GetReply_map =
{
	0x9F8BA01B,
	"GetReply",
	L"GetReplyResponse",
	sizeof("GetReply")-1,
	sizeof("GetReplyResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_GetReply_entries,
	sizeof(__CXMServer_GetReply_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_ValidateUserPerm_struct
{
	int sessionId;
	int permOper;
	bool ValidateUserPermResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_ValidateUserPerm_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_ValidateUserPerm_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xCD54DD0A, 
		"permOper", 
		L"permOper", 
		sizeof("permOper")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_ValidateUserPerm_struct, permOper),
		NULL,
		NULL,
		-1,
	},
	{
		0x809E3ABC, 
		"ValidateUserPermResult", 
		L"ValidateUserPermResult", 
		sizeof("ValidateUserPermResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_ValidateUserPerm_struct, ValidateUserPermResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_ValidateUserPerm_map =
{
	0x20BEFA0C,
	"ValidateUserPerm",
	L"ValidateUserPermResponse",
	sizeof("ValidateUserPerm")-1,
	sizeof("ValidateUserPermResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_ValidateUserPerm_entries,
	sizeof(__CXMServer_ValidateUserPerm_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_UpdateTaskDispatchArr_struct
{
	int sessionId;
	TaskDispatch *taskArr;
	int __taskArr_nSizeIs;
	int *UpdateTaskDispatchArrResult;
	int __UpdateTaskDispatchArrResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_UpdateTaskDispatchArr_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateTaskDispatchArr_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xCC04BE98, 
		"taskArr", 
		L"taskArr", 
		sizeof("taskArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_UpdateTaskDispatchArr_struct, taskArr),
		NULL,
		&__TaskDispatch_map,
		1+1,
	},
	{
		0xCC04BE98,
		"__taskArr_nSizeIs",
		L"__taskArr_nSizeIs",
		sizeof("__taskArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_UpdateTaskDispatchArr_struct, __taskArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x2D643E4A, 
		"UpdateTaskDispatchArrResult", 
		L"UpdateTaskDispatchArrResult", 
		sizeof("UpdateTaskDispatchArrResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_UpdateTaskDispatchArr_struct, UpdateTaskDispatchArrResult),
		NULL,
		NULL,
		3+1,
	},
	{
		0x2D643E4A,
		"__UpdateTaskDispatchArrResult_nSizeIs",
		L"__UpdateTaskDispatchArrResult_nSizeIs",
		sizeof("__UpdateTaskDispatchArrResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_UpdateTaskDispatchArr_struct, __UpdateTaskDispatchArrResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_UpdateTaskDispatchArr_map =
{
	0x1714191A,
	"UpdateTaskDispatchArr",
	L"UpdateTaskDispatchArrResponse",
	sizeof("UpdateTaskDispatchArr")-1,
	sizeof("UpdateTaskDispatchArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_UpdateTaskDispatchArr_entries,
	sizeof(__CXMServer_UpdateTaskDispatchArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateProduction_struct
{
	int sessionId;
	Production product;
	int SaveOrUpdateProductionResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateProduction_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateProduction_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xBF7820E1, 
		"product", 
		L"product", 
		sizeof("product")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateProduction_struct, product),
		NULL,
		&__Production_map,
		-1,
	},
	{
		0x88F09DB9, 
		"SaveOrUpdateProductionResult", 
		L"SaveOrUpdateProductionResult", 
		sizeof("SaveOrUpdateProductionResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateProduction_struct, SaveOrUpdateProductionResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateProduction_map =
{
	0x87361049,
	"SaveOrUpdateProduction",
	L"SaveOrUpdateProductionResponse",
	sizeof("SaveOrUpdateProduction")-1,
	sizeof("SaveOrUpdateProductionResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateProduction_entries,
	sizeof(__CXMServer_SaveOrUpdateProduction_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_InsertFeedBackArr_struct
{
	int sessionId;
	FeedBack *feedBackArr;
	int __feedBackArr_nSizeIs;
	bool InsertFeedBackArrResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_InsertFeedBackArr_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_InsertFeedBackArr_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xBBEE900A, 
		"feedBackArr", 
		L"feedBackArr", 
		sizeof("feedBackArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_InsertFeedBackArr_struct, feedBackArr),
		NULL,
		&__FeedBack_map,
		1+1,
	},
	{
		0xBBEE900A,
		"__feedBackArr_nSizeIs",
		L"__feedBackArr_nSizeIs",
		sizeof("__feedBackArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_InsertFeedBackArr_struct, __feedBackArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x9AB597DE, 
		"InsertFeedBackArrResult", 
		L"InsertFeedBackArrResult", 
		sizeof("InsertFeedBackArrResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_InsertFeedBackArr_struct, InsertFeedBackArrResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_InsertFeedBackArr_map =
{
	0x1E2227AE,
	"InsertFeedBackArr",
	L"InsertFeedBackArrResponse",
	sizeof("InsertFeedBackArr")-1,
	sizeof("InsertFeedBackArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_InsertFeedBackArr_entries,
	sizeof(__CXMServer_InsertFeedBackArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownloadProduction_struct
{
	int sessionId;
	int id;
	Production DownloadProductionResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownloadProduction_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadProduction_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadProduction_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x5AAF4D9E, 
		"DownloadProductionResult", 
		L"DownloadProductionResult", 
		sizeof("DownloadProductionResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadProduction_struct, DownloadProductionResult),
		NULL,
		&__Production_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownloadProduction_map =
{
	0xC3604D6E,
	"DownloadProduction",
	L"DownloadProductionResponse",
	sizeof("DownloadProduction")-1,
	sizeof("DownloadProductionResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownloadProduction_entries,
	sizeof(__CXMServer_DownloadProduction_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteWorkLog_struct
{
	int sessionId;
	int logId;
	bool DeleteWorkLogResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteWorkLog_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteWorkLog_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x07E0F46F, 
		"logId", 
		L"logId", 
		sizeof("logId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteWorkLog_struct, logId),
		NULL,
		NULL,
		-1,
	},
	{
		0x15B283B7, 
		"DeleteWorkLogResult", 
		L"DeleteWorkLogResult", 
		sizeof("DeleteWorkLogResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteWorkLog_struct, DeleteWorkLogResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteWorkLog_map =
{
	0x4C096DC7,
	"DeleteWorkLog",
	L"DeleteWorkLogResponse",
	sizeof("DeleteWorkLog")-1,
	sizeof("DeleteWorkLogResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteWorkLog_entries,
	sizeof(__CXMServer_DeleteWorkLog_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryTestItemArrBySetId_struct
{
	int sessionId;
	int setId;
	bool share;
	TestItem *QueryTestItemArrBySetIdResult;
	int __QueryTestItemArrBySetIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryTestItemArrBySetId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemArrBySetId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x085A5B79, 
		"setId", 
		L"setId", 
		sizeof("setId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemArrBySetId_struct, setId),
		NULL,
		NULL,
		-1,
	},
	{
		0x085BB513, 
		"share", 
		L"share", 
		sizeof("share")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemArrBySetId_struct, share),
		NULL,
		NULL,
		-1,
	},
	{
		0x02C47B5D, 
		"QueryTestItemArrBySetIdResult", 
		L"QueryTestItemArrBySetIdResult", 
		sizeof("QueryTestItemArrBySetIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryTestItemArrBySetId_struct, QueryTestItemArrBySetIdResult),
		NULL,
		&__TestItem_map,
		3+1,
	},
	{
		0x02C47B5D,
		"__QueryTestItemArrBySetIdResult_nSizeIs",
		L"__QueryTestItemArrBySetIdResult_nSizeIs",
		sizeof("__QueryTestItemArrBySetIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryTestItemArrBySetId_struct, __QueryTestItemArrBySetIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryTestItemArrBySetId_map =
{
	0xC577E6ED,
	"QueryTestItemArrBySetId",
	L"QueryTestItemArrBySetIdResponse",
	sizeof("QueryTestItemArrBySetId")-1,
	sizeof("QueryTestItemArrBySetIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryTestItemArrBySetId_entries,
	sizeof(__CXMServer_QueryTestItemArrBySetId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryClientProductByClientId_struct
{
	int sessionId;
	int clientId;
	Client_Production *QueryClientProductByClientIdResult;
	int __QueryClientProductByClientIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryClientProductByClientId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryClientProductByClientId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xD356924C, 
		"clientId", 
		L"clientId", 
		sizeof("clientId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryClientProductByClientId_struct, clientId),
		NULL,
		NULL,
		-1,
	},
	{
		0x8E6423BC, 
		"QueryClientProductByClientIdResult", 
		L"QueryClientProductByClientIdResult", 
		sizeof("QueryClientProductByClientIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryClientProductByClientId_struct, QueryClientProductByClientIdResult),
		NULL,
		&__Client_Production_map,
		2+1,
	},
	{
		0x8E6423BC,
		"__QueryClientProductByClientIdResult_nSizeIs",
		L"__QueryClientProductByClientIdResult_nSizeIs",
		sizeof("__QueryClientProductByClientIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryClientProductByClientId_struct, __QueryClientProductByClientIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryClientProductByClientId_map =
{
	0xB7A3230C,
	"QueryClientProductByClientId",
	L"QueryClientProductByClientIdResponse",
	sizeof("QueryClientProductByClientId")-1,
	sizeof("QueryClientProductByClientIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryClientProductByClientId_entries,
	sizeof(__CXMServer_QueryClientProductByClientId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryProductionByUserId_struct
{
	int sessionId;
	int userId;
	Production *QueryProductionByUserIdResult;
	int __QueryProductionByUserIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryProductionByUserId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryProductionByUserId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x1945E48C, 
		"userId", 
		L"userId", 
		sizeof("userId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryProductionByUserId_struct, userId),
		NULL,
		NULL,
		-1,
	},
	{
		0x8CAA7BC3, 
		"QueryProductionByUserIdResult", 
		L"QueryProductionByUserIdResult", 
		sizeof("QueryProductionByUserIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryProductionByUserId_struct, QueryProductionByUserIdResult),
		NULL,
		&__Production_map,
		2+1,
	},
	{
		0x8CAA7BC3,
		"__QueryProductionByUserIdResult_nSizeIs",
		L"__QueryProductionByUserIdResult_nSizeIs",
		sizeof("__QueryProductionByUserIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryProductionByUserId_struct, __QueryProductionByUserIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryProductionByUserId_map =
{
	0x60DF98D3,
	"QueryProductionByUserId",
	L"QueryProductionByUserIdResponse",
	sizeof("QueryProductionByUserId")-1,
	sizeof("QueryProductionByUserIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryProductionByUserId_entries,
	sizeof(__CXMServer_QueryProductionByUserId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryTestItemArrByCondition_struct
{
	int sessionId;
	int proId;
	int categoryId;
	bool share;
	TestItem *QueryTestItemArrByConditionResult;
	int __QueryTestItemArrByConditionResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryTestItemArrByCondition_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemArrByCondition_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x082B1D9E, 
		"proId", 
		L"proId", 
		sizeof("proId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemArrByCondition_struct, proId),
		NULL,
		NULL,
		-1,
	},
	{
		0x1827E7AB, 
		"categoryId", 
		L"categoryId", 
		sizeof("categoryId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemArrByCondition_struct, categoryId),
		NULL,
		NULL,
		-1,
	},
	{
		0x085BB513, 
		"share", 
		L"share", 
		sizeof("share")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemArrByCondition_struct, share),
		NULL,
		NULL,
		-1,
	},
	{
		0xD36DFDEB, 
		"QueryTestItemArrByConditionResult", 
		L"QueryTestItemArrByConditionResult", 
		sizeof("QueryTestItemArrByConditionResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryTestItemArrByCondition_struct, QueryTestItemArrByConditionResult),
		NULL,
		&__TestItem_map,
		4+1,
	},
	{
		0xD36DFDEB,
		"__QueryTestItemArrByConditionResult_nSizeIs",
		L"__QueryTestItemArrByConditionResult_nSizeIs",
		sizeof("__QueryTestItemArrByConditionResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryTestItemArrByCondition_struct, __QueryTestItemArrByConditionResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryTestItemArrByCondition_map =
{
	0x668C44FB,
	"QueryTestItemArrByCondition",
	L"QueryTestItemArrByConditionResponse",
	sizeof("QueryTestItemArrByCondition")-1,
	sizeof("QueryTestItemArrByConditionResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryTestItemArrByCondition_entries,
	sizeof(__CXMServer_QueryTestItemArrByCondition_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryproductionsByClientId_struct
{
	int sessionId;
	int clientId;
	Production *QueryproductionsByClientIdResult;
	int __QueryproductionsByClientIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryproductionsByClientId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryproductionsByClientId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xD356924C, 
		"clientId", 
		L"clientId", 
		sizeof("clientId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryproductionsByClientId_struct, clientId),
		NULL,
		NULL,
		-1,
	},
	{
		0x4BD50DB6, 
		"QueryproductionsByClientIdResult", 
		L"QueryproductionsByClientIdResult", 
		sizeof("QueryproductionsByClientIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryproductionsByClientId_struct, QueryproductionsByClientIdResult),
		NULL,
		&__Production_map,
		2+1,
	},
	{
		0x4BD50DB6,
		"__QueryproductionsByClientIdResult_nSizeIs",
		L"__QueryproductionsByClientIdResult_nSizeIs",
		sizeof("__QueryproductionsByClientIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryproductionsByClientId_struct, __QueryproductionsByClientIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryproductionsByClientId_map =
{
	0x94F67386,
	"QueryproductionsByClientId",
	L"QueryproductionsByClientIdResponse",
	sizeof("QueryproductionsByClientId")-1,
	sizeof("QueryproductionsByClientIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryproductionsByClientId_entries,
	sizeof(__CXMServer_QueryproductionsByClientId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownloadFeedBack_struct
{
	int sessionId;
	int id;
	FeedBack DownloadFeedBackResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownloadFeedBack_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadFeedBack_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadFeedBack_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x6F2E65FC, 
		"DownloadFeedBackResult", 
		L"DownloadFeedBackResult", 
		sizeof("DownloadFeedBackResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadFeedBack_struct, DownloadFeedBackResult),
		NULL,
		&__FeedBack_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownloadFeedBack_map =
{
	0xF406F54C,
	"DownloadFeedBack",
	L"DownloadFeedBackResponse",
	sizeof("DownloadFeedBack")-1,
	sizeof("DownloadFeedBackResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownloadFeedBack_entries,
	sizeof(__CXMServer_DownloadFeedBack_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_GetWorkLog_struct
{
	WorkLog GetWorkLogResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_GetWorkLog_entries[] =
{

	{
		0xB2620CC4, 
		"GetWorkLogResult", 
		L"GetWorkLogResult", 
		sizeof("GetWorkLogResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_GetWorkLog_struct, GetWorkLogResult),
		NULL,
		&__WorkLog_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_GetWorkLog_map =
{
	0xD2BF6E14,
	"GetWorkLog",
	L"GetWorkLogResponse",
	sizeof("GetWorkLog")-1,
	sizeof("GetWorkLogResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_GetWorkLog_entries,
	sizeof(__CXMServer_GetWorkLog_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteClientById_struct
{
	int sessionId;
	int id;
	bool DeleteClientByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteClientById_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteClientById_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteClientById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xA1C40E99, 
		"DeleteClientByIdResult", 
		L"DeleteClientByIdResult", 
		sizeof("DeleteClientByIdResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteClientById_struct, DeleteClientByIdResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteClientById_map =
{
	0x22A93929,
	"DeleteClientById",
	L"DeleteClientByIdResponse",
	sizeof("DeleteClientById")-1,
	sizeof("DeleteClientByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteClientById_entries,
	sizeof(__CXMServer_DeleteClientById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteTaskDispatch_struct
{
	int sessionId;
	int id;
	bool isGroup;
	bool DeleteTaskDispatchResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteTaskDispatch_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteTaskDispatch_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteTaskDispatch_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xA4296229, 
		"isGroup", 
		L"isGroup", 
		sizeof("isGroup")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteTaskDispatch_struct, isGroup),
		NULL,
		NULL,
		-1,
	},
	{
		0x6F8C0075, 
		"DeleteTaskDispatchResult", 
		L"DeleteTaskDispatchResult", 
		sizeof("DeleteTaskDispatchResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteTaskDispatch_struct, DeleteTaskDispatchResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteTaskDispatch_map =
{
	0x82351205,
	"DeleteTaskDispatch",
	L"DeleteTaskDispatchResponse",
	sizeof("DeleteTaskDispatch")-1,
	sizeof("DeleteTaskDispatchResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteTaskDispatch_entries,
	sizeof(__CXMServer_DeleteTaskDispatch_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_GetRole_struct
{
	Role GetRoleResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_GetRole_entries[] =
{

	{
		0x5FE49CB1, 
		"GetRoleResult", 
		L"GetRoleResult", 
		sizeof("GetRoleResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_GetRole_struct, GetRoleResult),
		NULL,
		&__Role_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_GetRole_map =
{
	0xEB25AD41,
	"GetRole",
	L"GetRoleResponse",
	sizeof("GetRole")-1,
	sizeof("GetRoleResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_GetRole_entries,
	sizeof(__CXMServer_GetRole_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryDownloadItemByClient_struct
{
	int sessionId;
	int clientId;
	DownloadItem *QueryDownloadItemByClientResult;
	int __QueryDownloadItemByClientResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryDownloadItemByClient_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryDownloadItemByClient_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xD356924C, 
		"clientId", 
		L"clientId", 
		sizeof("clientId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryDownloadItemByClient_struct, clientId),
		NULL,
		NULL,
		-1,
	},
	{
		0x8F5BD376, 
		"QueryDownloadItemByClientResult", 
		L"QueryDownloadItemByClientResult", 
		sizeof("QueryDownloadItemByClientResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryDownloadItemByClient_struct, QueryDownloadItemByClientResult),
		NULL,
		&__DownloadItem_map,
		2+1,
	},
	{
		0x8F5BD376,
		"__QueryDownloadItemByClientResult_nSizeIs",
		L"__QueryDownloadItemByClientResult_nSizeIs",
		sizeof("__QueryDownloadItemByClientResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryDownloadItemByClient_struct, __QueryDownloadItemByClientResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryDownloadItemByClient_map =
{
	0xD545A946,
	"QueryDownloadItemByClient",
	L"QueryDownloadItemByClientResponse",
	sizeof("QueryDownloadItemByClient")-1,
	sizeof("QueryDownloadItemByClientResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryDownloadItemByClient_entries,
	sizeof(__CXMServer_QueryDownloadItemByClient_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteDownloadItem_struct
{
	int sessionId;
	int id;
	bool DeleteDownloadItemResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteDownloadItem_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteDownloadItem_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteDownloadItem_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x15F683F9, 
		"DeleteDownloadItemResult", 
		L"DeleteDownloadItemResult", 
		sizeof("DeleteDownloadItemResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteDownloadItem_struct, DeleteDownloadItemResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteDownloadItem_map =
{
	0x6D4E8689,
	"DeleteDownloadItem",
	L"DeleteDownloadItemResponse",
	sizeof("DeleteDownloadItem")-1,
	sizeof("DeleteDownloadItemResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteDownloadItem_entries,
	sizeof(__CXMServer_DeleteDownloadItem_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteClient_DownloadItemByItem_struct
{
	int itemId;
	bool DeleteClient_DownloadItemByItemResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteClient_DownloadItemByItem_entries[] =
{

	{
		0xFD5A003C, 
		"itemId", 
		L"itemId", 
		sizeof("itemId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteClient_DownloadItemByItem_struct, itemId),
		NULL,
		NULL,
		-1,
	},
	{
		0x8C344381, 
		"DeleteClient_DownloadItemByItemResult", 
		L"DeleteClient_DownloadItemByItemResult", 
		sizeof("DeleteClient_DownloadItemByItemResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteClient_DownloadItemByItem_struct, DeleteClient_DownloadItemByItemResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteClient_DownloadItemByItem_map =
{
	0x69FA4811,
	"DeleteClient_DownloadItemByItem",
	L"DeleteClient_DownloadItemByItemResponse",
	sizeof("DeleteClient_DownloadItemByItem")-1,
	sizeof("DeleteClient_DownloadItemByItemResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteClient_DownloadItemByItem_entries,
	sizeof(__CXMServer_DeleteClient_DownloadItemByItem_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryEssentialFeedBacks_struct
{
	int productId;
	FeedBack *QueryEssentialFeedBacksResult;
	int __QueryEssentialFeedBacksResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryEssentialFeedBacks_entries[] =
{

	{
		0x7E03E6EE, 
		"productId", 
		L"productId", 
		sizeof("productId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryEssentialFeedBacks_struct, productId),
		NULL,
		NULL,
		-1,
	},
	{
		0x7A4B6615, 
		"QueryEssentialFeedBacksResult", 
		L"QueryEssentialFeedBacksResult", 
		sizeof("QueryEssentialFeedBacksResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryEssentialFeedBacks_struct, QueryEssentialFeedBacksResult),
		NULL,
		&__FeedBack_map,
		1+1,
	},
	{
		0x7A4B6615,
		"__QueryEssentialFeedBacksResult_nSizeIs",
		L"__QueryEssentialFeedBacksResult_nSizeIs",
		sizeof("__QueryEssentialFeedBacksResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryEssentialFeedBacks_struct, __QueryEssentialFeedBacksResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryEssentialFeedBacks_map =
{
	0x3A645FA5,
	"QueryEssentialFeedBacks",
	L"QueryEssentialFeedBacksResponse",
	sizeof("QueryEssentialFeedBacks")-1,
	sizeof("QueryEssentialFeedBacksResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryEssentialFeedBacks_entries,
	sizeof(__CXMServer_QueryEssentialFeedBacks_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryFeedBackArrByProcessState_struct
{
	int sessionId;
	bool state;
	FeedBack *QueryFeedBackArrByProcessStateResult;
	int __QueryFeedBackArrByProcessStateResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryFeedBackArrByProcessState_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrByProcessState_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x086249E1, 
		"state", 
		L"state", 
		sizeof("state")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrByProcessState_struct, state),
		NULL,
		NULL,
		-1,
	},
	{
		0xBEC6677A, 
		"QueryFeedBackArrByProcessStateResult", 
		L"QueryFeedBackArrByProcessStateResult", 
		sizeof("QueryFeedBackArrByProcessStateResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBackArrByProcessState_struct, QueryFeedBackArrByProcessStateResult),
		NULL,
		&__FeedBack_map,
		2+1,
	},
	{
		0xBEC6677A,
		"__QueryFeedBackArrByProcessStateResult_nSizeIs",
		L"__QueryFeedBackArrByProcessStateResult_nSizeIs",
		sizeof("__QueryFeedBackArrByProcessStateResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryFeedBackArrByProcessState_struct, __QueryFeedBackArrByProcessStateResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryFeedBackArrByProcessState_map =
{
	0x89A54E4A,
	"QueryFeedBackArrByProcessState",
	L"QueryFeedBackArrByProcessStateResponse",
	sizeof("QueryFeedBackArrByProcessState")-1,
	sizeof("QueryFeedBackArrByProcessStateResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryFeedBackArrByProcessState_entries,
	sizeof(__CXMServer_QueryFeedBackArrByProcessState_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryCategoryArrByObjType_struct
{
	int sessionId;
	int objType;
	Category *QueryCategoryArrByObjTypeResult;
	int __QueryCategoryArrByObjTypeResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryCategoryArrByObjType_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryCategoryArrByObjType_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x4CC82C1D, 
		"objType", 
		L"objType", 
		sizeof("objType")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryCategoryArrByObjType_struct, objType),
		NULL,
		NULL,
		-1,
	},
	{
		0xA16404F0, 
		"QueryCategoryArrByObjTypeResult", 
		L"QueryCategoryArrByObjTypeResult", 
		sizeof("QueryCategoryArrByObjTypeResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryCategoryArrByObjType_struct, QueryCategoryArrByObjTypeResult),
		NULL,
		&__Category_map,
		2+1,
	},
	{
		0xA16404F0,
		"__QueryCategoryArrByObjTypeResult_nSizeIs",
		L"__QueryCategoryArrByObjTypeResult_nSizeIs",
		sizeof("__QueryCategoryArrByObjTypeResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryCategoryArrByObjType_struct, __QueryCategoryArrByObjTypeResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryCategoryArrByObjType_map =
{
	0x8A202140,
	"QueryCategoryArrByObjType",
	L"QueryCategoryArrByObjTypeResponse",
	sizeof("QueryCategoryArrByObjType")-1,
	sizeof("QueryCategoryArrByObjTypeResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryCategoryArrByObjType_entries,
	sizeof(__CXMServer_QueryCategoryArrByObjType_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateJob_struct
{
	int sessionId;
	Job job;
	int SaveOrUpdateJobResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateJob_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateJob_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001D19B, 
		"job", 
		L"job", 
		sizeof("job")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateJob_struct, job),
		NULL,
		&__Job_map,
		-1,
	},
	{
		0xE7AB1B0D, 
		"SaveOrUpdateJobResult", 
		L"SaveOrUpdateJobResult", 
		sizeof("SaveOrUpdateJobResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateJob_struct, SaveOrUpdateJobResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateJob_map =
{
	0x7E85329D,
	"SaveOrUpdateJob",
	L"SaveOrUpdateJobResponse",
	sizeof("SaveOrUpdateJob")-1,
	sizeof("SaveOrUpdateJobResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateJob_entries,
	sizeof(__CXMServer_SaveOrUpdateJob_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryFeedBacksToDataSet_struct
{
	int sessionId;
	int promulgatorId;
	bool isUnSolve;
	bool isSolve;
	bool isGroup;
	BSTR QueryFeedBacksToDataSetResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryFeedBacksToDataSet_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBacksToDataSet_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xDDB05229, 
		"promulgatorId", 
		L"promulgatorId", 
		sizeof("promulgatorId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBacksToDataSet_struct, promulgatorId),
		NULL,
		NULL,
		-1,
	},
	{
		0x7E6746A8, 
		"isUnSolve", 
		L"isUnSolve", 
		sizeof("isUnSolve")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBacksToDataSet_struct, isUnSolve),
		NULL,
		NULL,
		-1,
	},
	{
		0xA500D665, 
		"isSolve", 
		L"isSolve", 
		sizeof("isSolve")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBacksToDataSet_struct, isSolve),
		NULL,
		NULL,
		-1,
	},
	{
		0xA4296229, 
		"isGroup", 
		L"isGroup", 
		sizeof("isGroup")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBacksToDataSet_struct, isGroup),
		NULL,
		NULL,
		-1,
	},
	{
		0xFE612536, 
		"QueryFeedBacksToDataSetResult", 
		L"QueryFeedBacksToDataSetResult", 
		sizeof("QueryFeedBacksToDataSetResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBacksToDataSet_struct, QueryFeedBacksToDataSetResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryFeedBacksToDataSet_map =
{
	0x1AE66B06,
	"QueryFeedBacksToDataSet",
	L"QueryFeedBacksToDataSetResponse",
	sizeof("QueryFeedBacksToDataSet")-1,
	sizeof("QueryFeedBacksToDataSetResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryFeedBacksToDataSet_entries,
	sizeof(__CXMServer_QueryFeedBacksToDataSet_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateNotification_struct
{
	int sessionId;
	Notification notification;
	bool isFile;
	int SaveOrUpdateNotificationResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateNotification_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateNotification_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x1A81F887, 
		"notification", 
		L"notification", 
		sizeof("notification")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateNotification_struct, notification),
		NULL,
		&__Notification_map,
		-1,
	},
	{
		0xFD36DB7C, 
		"isFile", 
		L"isFile", 
		sizeof("isFile")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateNotification_struct, isFile),
		NULL,
		NULL,
		-1,
	},
	{
		0x509BE859, 
		"SaveOrUpdateNotificationResult", 
		L"SaveOrUpdateNotificationResult", 
		sizeof("SaveOrUpdateNotificationResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateNotification_struct, SaveOrUpdateNotificationResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateNotification_map =
{
	0xE6DE82E9,
	"SaveOrUpdateNotification",
	L"SaveOrUpdateNotificationResponse",
	sizeof("SaveOrUpdateNotification")-1,
	sizeof("SaveOrUpdateNotificationResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateNotification_entries,
	sizeof(__CXMServer_SaveOrUpdateNotification_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_InsertUser_Role_struct
{
	int sessionId;
	int userId;
	Role *roleArr;
	int __roleArr_nSizeIs;
	bool InsertUser_RoleResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_InsertUser_Role_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_InsertUser_Role_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x1945E48C, 
		"userId", 
		L"userId", 
		sizeof("userId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_InsertUser_Role_struct, userId),
		NULL,
		NULL,
		-1,
	},
	{
		0x52368817, 
		"roleArr", 
		L"roleArr", 
		sizeof("roleArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_InsertUser_Role_struct, roleArr),
		NULL,
		&__Role_map,
		2+1,
	},
	{
		0x52368817,
		"__roleArr_nSizeIs",
		L"__roleArr_nSizeIs",
		sizeof("__roleArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_InsertUser_Role_struct, __roleArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x30D67EE4, 
		"InsertUser_RoleResult", 
		L"InsertUser_RoleResult", 
		sizeof("InsertUser_RoleResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_InsertUser_Role_struct, InsertUser_RoleResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_InsertUser_Role_map =
{
	0xC018E834,
	"InsertUser_Role",
	L"InsertUser_RoleResponse",
	sizeof("InsertUser_Role")-1,
	sizeof("InsertUser_RoleResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_InsertUser_Role_entries,
	sizeof(__CXMServer_InsertUser_Role_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateVersionRevision_struct
{
	int sessionId;
	VersionRevision version;
	int SaveOrUpdateVersionRevisionResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateVersionRevision_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateVersionRevision_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x6F40CEE6, 
		"version", 
		L"version", 
		sizeof("version")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateVersionRevision_struct, version),
		NULL,
		&__VersionRevision_map,
		-1,
	},
	{
		0x7DD56AA7, 
		"SaveOrUpdateVersionRevisionResult", 
		L"SaveOrUpdateVersionRevisionResult", 
		sizeof("SaveOrUpdateVersionRevisionResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateVersionRevision_struct, SaveOrUpdateVersionRevisionResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateVersionRevision_map =
{
	0x4881D0B7,
	"SaveOrUpdateVersionRevision",
	L"SaveOrUpdateVersionRevisionResponse",
	sizeof("SaveOrUpdateVersionRevision")-1,
	sizeof("SaveOrUpdateVersionRevisionResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateVersionRevision_entries,
	sizeof(__CXMServer_SaveOrUpdateVersionRevision_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryTestItemByWorkLogDate_struct
{
	int sessionId;
	BSTR startDate;
	BSTR endDate;
	TestItem *QueryTestItemByWorkLogDateResult;
	int __QueryTestItemByWorkLogDateResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryTestItemByWorkLogDate_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemByWorkLogDate_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x9ED2666C, 
		"startDate", 
		L"startDate", 
		sizeof("startDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemByWorkLogDate_struct, startDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x6689F575, 
		"endDate", 
		L"endDate", 
		sizeof("endDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemByWorkLogDate_struct, endDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x00A48182, 
		"QueryTestItemByWorkLogDateResult", 
		L"QueryTestItemByWorkLogDateResult", 
		sizeof("QueryTestItemByWorkLogDateResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryTestItemByWorkLogDate_struct, QueryTestItemByWorkLogDateResult),
		NULL,
		&__TestItem_map,
		3+1,
	},
	{
		0x00A48182,
		"__QueryTestItemByWorkLogDateResult_nSizeIs",
		L"__QueryTestItemByWorkLogDateResult_nSizeIs",
		sizeof("__QueryTestItemByWorkLogDateResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryTestItemByWorkLogDate_struct, __QueryTestItemByWorkLogDateResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryTestItemByWorkLogDate_map =
{
	0xBB720A52,
	"QueryTestItemByWorkLogDate",
	L"QueryTestItemByWorkLogDateResponse",
	sizeof("QueryTestItemByWorkLogDate")-1,
	sizeof("QueryTestItemByWorkLogDateResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryTestItemByWorkLogDate_entries,
	sizeof(__CXMServer_QueryTestItemByWorkLogDate_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryFeedBackArrByName_struct
{
	int sessionId;
	BSTR name;
	FeedBack *QueryFeedBackArrByNameResult;
	int __QueryFeedBackArrByNameResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryFeedBackArrByName_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrByName_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x003DFCC1, 
		"name", 
		L"name", 
		sizeof("name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBackArrByName_struct, name),
		NULL,
		NULL,
		-1,
	},
	{
		0x9E377A5B, 
		"QueryFeedBackArrByNameResult", 
		L"QueryFeedBackArrByNameResult", 
		sizeof("QueryFeedBackArrByNameResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBackArrByName_struct, QueryFeedBackArrByNameResult),
		NULL,
		&__FeedBack_map,
		2+1,
	},
	{
		0x9E377A5B,
		"__QueryFeedBackArrByNameResult_nSizeIs",
		L"__QueryFeedBackArrByNameResult_nSizeIs",
		sizeof("__QueryFeedBackArrByNameResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryFeedBackArrByName_struct, __QueryFeedBackArrByNameResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryFeedBackArrByName_map =
{
	0x09A69D6B,
	"QueryFeedBackArrByName",
	L"QueryFeedBackArrByNameResponse",
	sizeof("QueryFeedBackArrByName")-1,
	sizeof("QueryFeedBackArrByNameResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryFeedBackArrByName_entries,
	sizeof(__CXMServer_QueryFeedBackArrByName_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteUser_struct
{
	int sessionId;
	int id;
	bool DeleteUserResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteUser_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteUser_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteUser_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xB439FB91, 
		"DeleteUserResult", 
		L"DeleteUserResult", 
		sizeof("DeleteUserResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteUser_struct, DeleteUserResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteUser_map =
{
	0xAA4E4421,
	"DeleteUser",
	L"DeleteUserResponse",
	sizeof("DeleteUser")-1,
	sizeof("DeleteUserResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteUser_entries,
	sizeof(__CXMServer_DeleteUser_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateCategory_struct
{
	int sessionId;
	Category category;
	int SaveOrUpdateCategoryResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateCategory_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateCategory_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x9E3ACE5E, 
		"category", 
		L"category", 
		sizeof("category")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateCategory_struct, category),
		NULL,
		&__Category_map,
		-1,
	},
	{
		0xF75941F0, 
		"SaveOrUpdateCategoryResult", 
		L"SaveOrUpdateCategoryResult", 
		sizeof("SaveOrUpdateCategoryResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateCategory_struct, SaveOrUpdateCategoryResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateCategory_map =
{
	0x32589E40,
	"SaveOrUpdateCategory",
	L"SaveOrUpdateCategoryResponse",
	sizeof("SaveOrUpdateCategory")-1,
	sizeof("SaveOrUpdateCategoryResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateCategory_entries,
	sizeof(__CXMServer_SaveOrUpdateCategory_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownloadClientProductById_struct
{
	int id;
	Client_Production DownloadClientProductByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownloadClientProductById_entries[] =
{

	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadClientProductById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xFBCE26DF, 
		"DownloadClientProductByIdResult", 
		L"DownloadClientProductByIdResult", 
		sizeof("DownloadClientProductByIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadClientProductById_struct, DownloadClientProductByIdResult),
		NULL,
		&__Client_Production_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownloadClientProductById_map =
{
	0x279A7AEF,
	"DownloadClientProductById",
	L"DownloadClientProductByIdResponse",
	sizeof("DownloadClientProductById")-1,
	sizeof("DownloadClientProductByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownloadClientProductById_entries,
	sizeof(__CXMServer_DownloadClientProductById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryLatestVersionRevisionArr_struct
{
	int sessionId;
	VersionRevision *QueryLatestVersionRevisionArrResult;
	int __QueryLatestVersionRevisionArrResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryLatestVersionRevisionArr_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryLatestVersionRevisionArr_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x14242B3C, 
		"QueryLatestVersionRevisionArrResult", 
		L"QueryLatestVersionRevisionArrResult", 
		sizeof("QueryLatestVersionRevisionArrResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryLatestVersionRevisionArr_struct, QueryLatestVersionRevisionArrResult),
		NULL,
		&__VersionRevision_map,
		1+1,
	},
	{
		0x14242B3C,
		"__QueryLatestVersionRevisionArrResult_nSizeIs",
		L"__QueryLatestVersionRevisionArrResult_nSizeIs",
		sizeof("__QueryLatestVersionRevisionArrResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryLatestVersionRevisionArr_struct, __QueryLatestVersionRevisionArrResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryLatestVersionRevisionArr_map =
{
	0xAD830A8C,
	"QueryLatestVersionRevisionArr",
	L"QueryLatestVersionRevisionArrResponse",
	sizeof("QueryLatestVersionRevisionArr")-1,
	sizeof("QueryLatestVersionRevisionArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryLatestVersionRevisionArr_entries,
	sizeof(__CXMServer_QueryLatestVersionRevisionArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteReply_struct
{
	int sessionId;
	int id;
	bool DeleteReplyResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteReply_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteReply_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteReply_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x776C2F3E, 
		"DeleteReplyResult", 
		L"DeleteReplyResult", 
		sizeof("DeleteReplyResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteReply_struct, DeleteReplyResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteReply_map =
{
	0x02DC170E,
	"DeleteReply",
	L"DeleteReplyResponse",
	sizeof("DeleteReply")-1,
	sizeof("DeleteReplyResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteReply_entries,
	sizeof(__CXMServer_DeleteReply_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryTaskDispatchs_struct
{
	int sessionId;
	BSTR keyword;
	int promulgatorId;
	int acceptUserId;
	int task_state;
	BSTR startDate;
	BSTR endDate;
	int taskProId;
	TaskDispatch *QueryTaskDispatchsResult;
	int __QueryTaskDispatchsResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryTaskDispatchs_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTaskDispatchs_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x21012985, 
		"keyword", 
		L"keyword", 
		sizeof("keyword")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryTaskDispatchs_struct, keyword),
		NULL,
		NULL,
		-1,
	},
	{
		0xDDB05229, 
		"promulgatorId", 
		L"promulgatorId", 
		sizeof("promulgatorId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTaskDispatchs_struct, promulgatorId),
		NULL,
		NULL,
		-1,
	},
	{
		0xB110A95C, 
		"acceptUserId", 
		L"acceptUserId", 
		sizeof("acceptUserId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTaskDispatchs_struct, acceptUserId),
		NULL,
		NULL,
		-1,
	},
	{
		0x2C16F7B3, 
		"task_state", 
		L"task_state", 
		sizeof("task_state")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTaskDispatchs_struct, task_state),
		NULL,
		NULL,
		-1,
	},
	{
		0x9ED2666C, 
		"startDate", 
		L"startDate", 
		sizeof("startDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTaskDispatchs_struct, startDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x6689F575, 
		"endDate", 
		L"endDate", 
		sizeof("endDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTaskDispatchs_struct, endDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xE13E3131, 
		"taskProId", 
		L"taskProId", 
		sizeof("taskProId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTaskDispatchs_struct, taskProId),
		NULL,
		NULL,
		-1,
	},
	{
		0x9A227F6B, 
		"QueryTaskDispatchsResult", 
		L"QueryTaskDispatchsResult", 
		sizeof("QueryTaskDispatchsResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryTaskDispatchs_struct, QueryTaskDispatchsResult),
		NULL,
		&__TaskDispatch_map,
		8+1,
	},
	{
		0x9A227F6B,
		"__QueryTaskDispatchsResult_nSizeIs",
		L"__QueryTaskDispatchsResult_nSizeIs",
		sizeof("__QueryTaskDispatchsResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryTaskDispatchs_struct, __QueryTaskDispatchsResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryTaskDispatchs_map =
{
	0xAC67267B,
	"QueryTaskDispatchs",
	L"QueryTaskDispatchsResponse",
	sizeof("QueryTaskDispatchs")-1,
	sizeof("QueryTaskDispatchsResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryTaskDispatchs_entries,
	sizeof(__CXMServer_QueryTaskDispatchs_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownloadAttendanceById_struct
{
	int sessionId;
	int id;
	Attendance DownloadAttendanceByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownloadAttendanceById_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadAttendanceById_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadAttendanceById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x70DA6776, 
		"DownloadAttendanceByIdResult", 
		L"DownloadAttendanceByIdResult", 
		sizeof("DownloadAttendanceByIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadAttendanceById_struct, DownloadAttendanceByIdResult),
		NULL,
		&__Attendance_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownloadAttendanceById_map =
{
	0x10B93D46,
	"DownloadAttendanceById",
	L"DownloadAttendanceByIdResponse",
	sizeof("DownloadAttendanceById")-1,
	sizeof("DownloadAttendanceByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownloadAttendanceById_entries,
	sizeof(__CXMServer_DownloadAttendanceById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownloadCategoryById_struct
{
	int sessionId;
	int id;
	Category DownloadCategoryByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownloadCategoryById_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadCategoryById_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadCategoryById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x9D63431D, 
		"DownloadCategoryByIdResult", 
		L"DownloadCategoryByIdResult", 
		sizeof("DownloadCategoryByIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadCategoryById_struct, DownloadCategoryByIdResult),
		NULL,
		&__Category_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownloadCategoryById_map =
{
	0x82E79EAD,
	"DownloadCategoryById",
	L"DownloadCategoryByIdResponse",
	sizeof("DownloadCategoryById")-1,
	sizeof("DownloadCategoryByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownloadCategoryById_entries,
	sizeof(__CXMServer_DownloadCategoryById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryDevelopLogArr_struct
{
	int sessionId;
	int *productIdArr;
	int __productIdArr_nSizeIs;
	int userId;
	BSTR startDate;
	BSTR endDate;
	DevelopLog *QueryDevelopLogArrResult;
	int __QueryDevelopLogArrResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryDevelopLogArr_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryDevelopLogArr_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xE1C5CBD3, 
		"productIdArr", 
		L"productIdArr", 
		sizeof("productIdArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryDevelopLogArr_struct, productIdArr),
		NULL,
		NULL,
		1+1,
	},
	{
		0xE1C5CBD3,
		"__productIdArr_nSizeIs",
		L"__productIdArr_nSizeIs",
		sizeof("__productIdArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryDevelopLogArr_struct, __productIdArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1945E48C, 
		"userId", 
		L"userId", 
		sizeof("userId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryDevelopLogArr_struct, userId),
		NULL,
		NULL,
		-1,
	},
	{
		0x9ED2666C, 
		"startDate", 
		L"startDate", 
		sizeof("startDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryDevelopLogArr_struct, startDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x6689F575, 
		"endDate", 
		L"endDate", 
		sizeof("endDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryDevelopLogArr_struct, endDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x5611452B, 
		"QueryDevelopLogArrResult", 
		L"QueryDevelopLogArrResult", 
		sizeof("QueryDevelopLogArrResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryDevelopLogArr_struct, QueryDevelopLogArrResult),
		NULL,
		&__DevelopLog_map,
		6+1,
	},
	{
		0x5611452B,
		"__QueryDevelopLogArrResult_nSizeIs",
		L"__QueryDevelopLogArrResult_nSizeIs",
		sizeof("__QueryDevelopLogArrResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryDevelopLogArr_struct, __QueryDevelopLogArrResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryDevelopLogArr_map =
{
	0x1F1E5C3B,
	"QueryDevelopLogArr",
	L"QueryDevelopLogArrResponse",
	sizeof("QueryDevelopLogArr")-1,
	sizeof("QueryDevelopLogArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryDevelopLogArr_entries,
	sizeof(__CXMServer_QueryDevelopLogArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownloadItemById_struct
{
	int id;
	DownloadItem DownloadItemByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownloadItemById_entries[] =
{

	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadItemById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x49E650AE, 
		"DownloadItemByIdResult", 
		L"DownloadItemByIdResult", 
		sizeof("DownloadItemByIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadItemById_struct, DownloadItemByIdResult),
		NULL,
		&__DownloadItem_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownloadItemById_map =
{
	0x5C64547E,
	"DownloadItemById",
	L"DownloadItemByIdResponse",
	sizeof("DownloadItemById")-1,
	sizeof("DownloadItemByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownloadItemById_entries,
	sizeof(__CXMServer_DownloadItemById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_InsertTaskDispatchArr_struct
{
	int sessionId;
	TaskDispatch *taskArr;
	int __taskArr_nSizeIs;
	int *InsertTaskDispatchArrResult;
	int __InsertTaskDispatchArrResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_InsertTaskDispatchArr_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_InsertTaskDispatchArr_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xCC04BE98, 
		"taskArr", 
		L"taskArr", 
		sizeof("taskArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_InsertTaskDispatchArr_struct, taskArr),
		NULL,
		&__TaskDispatch_map,
		1+1,
	},
	{
		0xCC04BE98,
		"__taskArr_nSizeIs",
		L"__taskArr_nSizeIs",
		sizeof("__taskArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_InsertTaskDispatchArr_struct, __taskArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x5E319B7C, 
		"InsertTaskDispatchArrResult", 
		L"InsertTaskDispatchArrResult", 
		sizeof("InsertTaskDispatchArrResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_InsertTaskDispatchArr_struct, InsertTaskDispatchArrResult),
		NULL,
		NULL,
		3+1,
	},
	{
		0x5E319B7C,
		"__InsertTaskDispatchArrResult_nSizeIs",
		L"__InsertTaskDispatchArrResult_nSizeIs",
		sizeof("__InsertTaskDispatchArrResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_InsertTaskDispatchArr_struct, __InsertTaskDispatchArrResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_InsertTaskDispatchArr_map =
{
	0xB0AD8ACC,
	"InsertTaskDispatchArr",
	L"InsertTaskDispatchArrResponse",
	sizeof("InsertTaskDispatchArr")-1,
	sizeof("InsertTaskDispatchArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_InsertTaskDispatchArr_entries,
	sizeof(__CXMServer_InsertTaskDispatchArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownLoadLasrVersionByProId_struct
{
	int sessionId;
	int productionId;
	VersionRevision DownLoadLasrVersionByProIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownLoadLasrVersionByProId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadLasrVersionByProId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xE40F9E34, 
		"productionId", 
		L"productionId", 
		sizeof("productionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadLasrVersionByProId_struct, productionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xB7D93E08, 
		"DownLoadLasrVersionByProIdResult", 
		L"DownLoadLasrVersionByProIdResult", 
		sizeof("DownLoadLasrVersionByProIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadLasrVersionByProId_struct, DownLoadLasrVersionByProIdResult),
		NULL,
		&__VersionRevision_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownLoadLasrVersionByProId_map =
{
	0x12C80058,
	"DownLoadLasrVersionByProId",
	L"DownLoadLasrVersionByProIdResponse",
	sizeof("DownLoadLasrVersionByProId")-1,
	sizeof("DownLoadLasrVersionByProIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownLoadLasrVersionByProId_entries,
	sizeof(__CXMServer_DownLoadLasrVersionByProId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryClientByProvinceNum_struct
{
	int sessionId;
	int num;
	Client *QueryClientByProvinceNumResult;
	int __QueryClientByProvinceNumResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryClientByProvinceNum_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryClientByProvinceNum_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001E370, 
		"num", 
		L"num", 
		sizeof("num")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryClientByProvinceNum_struct, num),
		NULL,
		NULL,
		-1,
	},
	{
		0x4C3355E5, 
		"QueryClientByProvinceNumResult", 
		L"QueryClientByProvinceNumResult", 
		sizeof("QueryClientByProvinceNumResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryClientByProvinceNum_struct, QueryClientByProvinceNumResult),
		NULL,
		&__Client_map,
		2+1,
	},
	{
		0x4C3355E5,
		"__QueryClientByProvinceNumResult_nSizeIs",
		L"__QueryClientByProvinceNumResult_nSizeIs",
		sizeof("__QueryClientByProvinceNumResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryClientByProvinceNum_struct, __QueryClientByProvinceNumResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryClientByProvinceNum_map =
{
	0x26078375,
	"QueryClientByProvinceNum",
	L"QueryClientByProvinceNumResponse",
	sizeof("QueryClientByProvinceNum")-1,
	sizeof("QueryClientByProvinceNumResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryClientByProvinceNum_entries,
	sizeof(__CXMServer_QueryClientByProvinceNum_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteWorkLogById_struct
{
	int sessionId;
	int id;
	bool DeleteWorkLogByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteWorkLogById_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteWorkLogById_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteWorkLogById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xAA5BD33F, 
		"DeleteWorkLogByIdResult", 
		L"DeleteWorkLogByIdResult", 
		sizeof("DeleteWorkLogByIdResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteWorkLogById_struct, DeleteWorkLogByIdResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteWorkLogById_map =
{
	0xB044BF4F,
	"DeleteWorkLogById",
	L"DeleteWorkLogByIdResponse",
	sizeof("DeleteWorkLogById")-1,
	sizeof("DeleteWorkLogByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteWorkLogById_entries,
	sizeof(__CXMServer_DeleteWorkLogById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryFeedBackArrByProposerId_struct
{
	int sessionId;
	int proposerId;
	FeedBack *QueryFeedBackArrByProposerIdResult;
	int __QueryFeedBackArrByProposerIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryFeedBackArrByProposerId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrByProposerId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xCD56A3A7, 
		"proposerId", 
		L"proposerId", 
		sizeof("proposerId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrByProposerId_struct, proposerId),
		NULL,
		NULL,
		-1,
	},
	{
		0x83BF1601, 
		"QueryFeedBackArrByProposerIdResult", 
		L"QueryFeedBackArrByProposerIdResult", 
		sizeof("QueryFeedBackArrByProposerIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBackArrByProposerId_struct, QueryFeedBackArrByProposerIdResult),
		NULL,
		&__FeedBack_map,
		2+1,
	},
	{
		0x83BF1601,
		"__QueryFeedBackArrByProposerIdResult_nSizeIs",
		L"__QueryFeedBackArrByProposerIdResult_nSizeIs",
		sizeof("__QueryFeedBackArrByProposerIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryFeedBackArrByProposerId_struct, __QueryFeedBackArrByProposerIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryFeedBackArrByProposerId_map =
{
	0x6F83BA91,
	"QueryFeedBackArrByProposerId",
	L"QueryFeedBackArrByProposerIdResponse",
	sizeof("QueryFeedBackArrByProposerId")-1,
	sizeof("QueryFeedBackArrByProposerIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryFeedBackArrByProposerId_entries,
	sizeof(__CXMServer_QueryFeedBackArrByProposerId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownLoadJobById_struct
{
	int sessionId;
	int id;
	Job DownLoadJobByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownLoadJobById_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadJobById_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadJobById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xFF91177A, 
		"DownLoadJobByIdResult", 
		L"DownLoadJobByIdResult", 
		sizeof("DownLoadJobByIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadJobById_struct, DownLoadJobByIdResult),
		NULL,
		&__Job_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownLoadJobById_map =
{
	0x27DBFE4A,
	"DownLoadJobById",
	L"DownLoadJobByIdResponse",
	sizeof("DownLoadJobById")-1,
	sizeof("DownLoadJobByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownLoadJobById_entries,
	sizeof(__CXMServer_DownLoadJobById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateTestItem_struct
{
	int sessionId;
	TestItem ti;
	int SaveOrUpdateTestItemResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateTestItem_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateTestItem_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000F5D, 
		"ti", 
		L"ti", 
		sizeof("ti")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateTestItem_struct, ti),
		NULL,
		&__TestItem_map,
		-1,
	},
	{
		0xF5B307C1, 
		"SaveOrUpdateTestItemResult", 
		L"SaveOrUpdateTestItemResult", 
		sizeof("SaveOrUpdateTestItemResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateTestItem_struct, SaveOrUpdateTestItemResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateTestItem_map =
{
	0x2E3B1C51,
	"SaveOrUpdateTestItem",
	L"SaveOrUpdateTestItemResponse",
	sizeof("SaveOrUpdateTestItem")-1,
	sizeof("SaveOrUpdateTestItemResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateTestItem_entries,
	sizeof(__CXMServer_SaveOrUpdateTestItem_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryAllJobs_struct
{
	int sessionId;
	Job *QueryAllJobsResult;
	int __QueryAllJobsResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryAllJobs_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryAllJobs_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x84EBE5BC, 
		"QueryAllJobsResult", 
		L"QueryAllJobsResult", 
		sizeof("QueryAllJobsResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryAllJobs_struct, QueryAllJobsResult),
		NULL,
		&__Job_map,
		1+1,
	},
	{
		0x84EBE5BC,
		"__QueryAllJobsResult_nSizeIs",
		L"__QueryAllJobsResult_nSizeIs",
		sizeof("__QueryAllJobsResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryAllJobs_struct, __QueryAllJobsResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryAllJobs_map =
{
	0x6F23650C,
	"QueryAllJobs",
	L"QueryAllJobsResponse",
	sizeof("QueryAllJobs")-1,
	sizeof("QueryAllJobsResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryAllJobs_entries,
	sizeof(__CXMServer_QueryAllJobs_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryPartBuildByPartItemId_struct
{
	int partItemId;
	PartBuild *QueryPartBuildByPartItemIdResult;
	int __QueryPartBuildByPartItemIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryPartBuildByPartItemId_entries[] =
{

	{
		0xB12F8F93, 
		"partItemId", 
		L"partItemId", 
		sizeof("partItemId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryPartBuildByPartItemId_struct, partItemId),
		NULL,
		NULL,
		-1,
	},
	{
		0xC6BA81CA, 
		"QueryPartBuildByPartItemIdResult", 
		L"QueryPartBuildByPartItemIdResult", 
		sizeof("QueryPartBuildByPartItemIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryPartBuildByPartItemId_struct, QueryPartBuildByPartItemIdResult),
		NULL,
		&__PartBuild_map,
		1+1,
	},
	{
		0xC6BA81CA,
		"__QueryPartBuildByPartItemIdResult_nSizeIs",
		L"__QueryPartBuildByPartItemIdResult_nSizeIs",
		sizeof("__QueryPartBuildByPartItemIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryPartBuildByPartItemId_struct, __QueryPartBuildByPartItemIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryPartBuildByPartItemId_map =
{
	0x5F093C9A,
	"QueryPartBuildByPartItemId",
	L"QueryPartBuildByPartItemIdResponse",
	sizeof("QueryPartBuildByPartItemId")-1,
	sizeof("QueryPartBuildByPartItemIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryPartBuildByPartItemId_entries,
	sizeof(__CXMServer_QueryPartBuildByPartItemId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryRolesByUserId_struct
{
	int userId;
	Role *QueryRolesByUserIdResult;
	int __QueryRolesByUserIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryRolesByUserId_entries[] =
{

	{
		0x1945E48C, 
		"userId", 
		L"userId", 
		sizeof("userId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryRolesByUserId_struct, userId),
		NULL,
		NULL,
		-1,
	},
	{
		0x6EE1C441, 
		"QueryRolesByUserIdResult", 
		L"QueryRolesByUserIdResult", 
		sizeof("QueryRolesByUserIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryRolesByUserId_struct, QueryRolesByUserIdResult),
		NULL,
		&__Role_map,
		1+1,
	},
	{
		0x6EE1C441,
		"__QueryRolesByUserIdResult_nSizeIs",
		L"__QueryRolesByUserIdResult_nSizeIs",
		sizeof("__QueryRolesByUserIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryRolesByUserId_struct, __QueryRolesByUserIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryRolesByUserId_map =
{
	0xAE0AF8D1,
	"QueryRolesByUserId",
	L"QueryRolesByUserIdResponse",
	sizeof("QueryRolesByUserId")-1,
	sizeof("QueryRolesByUserIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryRolesByUserId_entries,
	sizeof(__CXMServer_QueryRolesByUserId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryFeedBackArrByProblemGrade_struct
{
	int sessionId;
	int problemGrade;
	FeedBack *QueryFeedBackArrByProblemGradeResult;
	int __QueryFeedBackArrByProblemGradeResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryFeedBackArrByProblemGrade_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrByProblemGrade_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x3400CD94, 
		"problemGrade", 
		L"problemGrade", 
		sizeof("problemGrade")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrByProblemGrade_struct, problemGrade),
		NULL,
		NULL,
		-1,
	},
	{
		0x30CEE86E, 
		"QueryFeedBackArrByProblemGradeResult", 
		L"QueryFeedBackArrByProblemGradeResult", 
		sizeof("QueryFeedBackArrByProblemGradeResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBackArrByProblemGrade_struct, QueryFeedBackArrByProblemGradeResult),
		NULL,
		&__FeedBack_map,
		2+1,
	},
	{
		0x30CEE86E,
		"__QueryFeedBackArrByProblemGradeResult_nSizeIs",
		L"__QueryFeedBackArrByProblemGradeResult_nSizeIs",
		sizeof("__QueryFeedBackArrByProblemGradeResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryFeedBackArrByProblemGrade_struct, __QueryFeedBackArrByProblemGradeResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryFeedBackArrByProblemGrade_map =
{
	0x9FD1DC3E,
	"QueryFeedBackArrByProblemGrade",
	L"QueryFeedBackArrByProblemGradeResponse",
	sizeof("QueryFeedBackArrByProblemGrade")-1,
	sizeof("QueryFeedBackArrByProblemGradeResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryFeedBackArrByProblemGrade_entries,
	sizeof(__CXMServer_QueryFeedBackArrByProblemGrade_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteTestItemSet_struct
{
	int sessionId;
	int id;
	bool DeleteTestItemSetResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteTestItemSet_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteTestItemSet_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteTestItemSet_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x365321CD, 
		"DeleteTestItemSetResult", 
		L"DeleteTestItemSetResult", 
		sizeof("DeleteTestItemSetResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteTestItemSet_struct, DeleteTestItemSetResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteTestItemSet_map =
{
	0x1749E95D,
	"DeleteTestItemSet",
	L"DeleteTestItemSetResponse",
	sizeof("DeleteTestItemSet")-1,
	sizeof("DeleteTestItemSetResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteTestItemSet_entries,
	sizeof(__CXMServer_DeleteTestItemSet_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryFeedBackArrByCondition_struct
{
	int sessionId;
	BSTR keyword;
	int gradeId;
	BSTR problemType;
	BSTR processState;
	int *productIdArr;
	int __productIdArr_nSizeIs;
	FeedBack *QueryFeedBackArrByConditionResult;
	int __QueryFeedBackArrByConditionResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryFeedBackArrByCondition_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrByCondition_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x21012985, 
		"keyword", 
		L"keyword", 
		sizeof("keyword")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBackArrByCondition_struct, keyword),
		NULL,
		NULL,
		-1,
	},
	{
		0x09AE5F90, 
		"gradeId", 
		L"gradeId", 
		sizeof("gradeId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrByCondition_struct, gradeId),
		NULL,
		NULL,
		-1,
	},
	{
		0x857BA3F3, 
		"problemType", 
		L"problemType", 
		sizeof("problemType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBackArrByCondition_struct, problemType),
		NULL,
		NULL,
		-1,
	},
	{
		0x7738F3A0, 
		"processState", 
		L"processState", 
		sizeof("processState")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBackArrByCondition_struct, processState),
		NULL,
		NULL,
		-1,
	},
	{
		0xE1C5CBD3, 
		"productIdArr", 
		L"productIdArr", 
		sizeof("productIdArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBackArrByCondition_struct, productIdArr),
		NULL,
		NULL,
		5+1,
	},
	{
		0xE1C5CBD3,
		"__productIdArr_nSizeIs",
		L"__productIdArr_nSizeIs",
		sizeof("__productIdArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryFeedBackArrByCondition_struct, __productIdArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x4DAB7801, 
		"QueryFeedBackArrByConditionResult", 
		L"QueryFeedBackArrByConditionResult", 
		sizeof("QueryFeedBackArrByConditionResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBackArrByCondition_struct, QueryFeedBackArrByConditionResult),
		NULL,
		&__FeedBack_map,
		7+1,
	},
	{
		0x4DAB7801,
		"__QueryFeedBackArrByConditionResult_nSizeIs",
		L"__QueryFeedBackArrByConditionResult_nSizeIs",
		sizeof("__QueryFeedBackArrByConditionResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryFeedBackArrByCondition_struct, __QueryFeedBackArrByConditionResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryFeedBackArrByCondition_map =
{
	0x66109C91,
	"QueryFeedBackArrByCondition",
	L"QueryFeedBackArrByConditionResponse",
	sizeof("QueryFeedBackArrByCondition")-1,
	sizeof("QueryFeedBackArrByConditionResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryFeedBackArrByCondition_entries,
	sizeof(__CXMServer_QueryFeedBackArrByCondition_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownloadVersionRevision_struct
{
	int sessionId;
	int id;
	VersionRevision DownloadVersionRevisionResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownloadVersionRevision_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadVersionRevision_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadVersionRevision_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x6346D1AC, 
		"DownloadVersionRevisionResult", 
		L"DownloadVersionRevisionResult", 
		sizeof("DownloadVersionRevisionResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadVersionRevision_struct, DownloadVersionRevisionResult),
		NULL,
		&__VersionRevision_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownloadVersionRevision_map =
{
	0x4FE90CFC,
	"DownloadVersionRevision",
	L"DownloadVersionRevisionResponse",
	sizeof("DownloadVersionRevision")-1,
	sizeof("DownloadVersionRevisionResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownloadVersionRevision_entries,
	sizeof(__CXMServer_DownloadVersionRevision_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteAttendance_struct
{
	int sessionId;
	int id;
	bool DeleteAttendanceResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteAttendance_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteAttendance_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteAttendance_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xA124EA49, 
		"DeleteAttendanceResult", 
		L"DeleteAttendanceResult", 
		sizeof("DeleteAttendanceResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteAttendance_struct, DeleteAttendanceResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteAttendance_map =
{
	0x7DAFC0D9,
	"DeleteAttendance",
	L"DeleteAttendanceResponse",
	sizeof("DeleteAttendance")-1,
	sizeof("DeleteAttendanceResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteAttendance_entries,
	sizeof(__CXMServer_DeleteAttendance_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_ValidateUserName_struct
{
	BSTR userName;
	bool ValidateUserNameResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_ValidateUserName_entries[] =
{

	{
		0x8253F720, 
		"userName", 
		L"userName", 
		sizeof("userName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_ValidateUserName_struct, userName),
		NULL,
		NULL,
		-1,
	},
	{
		0xA17C4609, 
		"ValidateUserNameResult", 
		L"ValidateUserNameResult", 
		sizeof("ValidateUserNameResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_ValidateUserName_struct, ValidateUserNameResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_ValidateUserName_map =
{
	0xF14D0C99,
	"ValidateUserName",
	L"ValidateUserNameResponse",
	sizeof("ValidateUserName")-1,
	sizeof("ValidateUserNameResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_ValidateUserName_entries,
	sizeof(__CXMServer_ValidateUserName_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteTestItemById_struct
{
	int sessionId;
	int id;
	bool DeleteTestItemByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteTestItemById_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteTestItemById_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteTestItemById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x62F5C209, 
		"DeleteTestItemByIdResult", 
		L"DeleteTestItemByIdResult", 
		sizeof("DeleteTestItemByIdResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteTestItemById_struct, DeleteTestItemByIdResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteTestItemById_map =
{
	0xF7158899,
	"DeleteTestItemById",
	L"DeleteTestItemByIdResponse",
	sizeof("DeleteTestItemById")-1,
	sizeof("DeleteTestItemByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteTestItemById_entries,
	sizeof(__CXMServer_DeleteTestItemById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownloadTaskDispatch_struct
{
	int sessionId;
	int id;
	TaskDispatch DownloadTaskDispatchResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownloadTaskDispatch_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadTaskDispatch_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadTaskDispatch_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x2873C7DA, 
		"DownloadTaskDispatchResult", 
		L"DownloadTaskDispatchResult", 
		sizeof("DownloadTaskDispatchResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadTaskDispatch_struct, DownloadTaskDispatchResult),
		NULL,
		&__TaskDispatch_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownloadTaskDispatch_map =
{
	0x142C46AA,
	"DownloadTaskDispatch",
	L"DownloadTaskDispatchResponse",
	sizeof("DownloadTaskDispatch")-1,
	sizeof("DownloadTaskDispatchResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownloadTaskDispatch_entries,
	sizeof(__CXMServer_DownloadTaskDispatch_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryFeedBacksCount_struct
{
	int sessionId;
	int gradeId;
	BSTR problemType;
	BSTR processState;
	int *productIdArr;
	int __productIdArr_nSizeIs;
	int QueryFeedBacksCountResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryFeedBacksCount_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBacksCount_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x09AE5F90, 
		"gradeId", 
		L"gradeId", 
		sizeof("gradeId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBacksCount_struct, gradeId),
		NULL,
		NULL,
		-1,
	},
	{
		0x857BA3F3, 
		"problemType", 
		L"problemType", 
		sizeof("problemType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBacksCount_struct, problemType),
		NULL,
		NULL,
		-1,
	},
	{
		0x7738F3A0, 
		"processState", 
		L"processState", 
		sizeof("processState")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBacksCount_struct, processState),
		NULL,
		NULL,
		-1,
	},
	{
		0xE1C5CBD3, 
		"productIdArr", 
		L"productIdArr", 
		sizeof("productIdArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBacksCount_struct, productIdArr),
		NULL,
		NULL,
		4+1,
	},
	{
		0xE1C5CBD3,
		"__productIdArr_nSizeIs",
		L"__productIdArr_nSizeIs",
		sizeof("__productIdArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryFeedBacksCount_struct, __productIdArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x6ED8F276, 
		"QueryFeedBacksCountResult", 
		L"QueryFeedBacksCountResult", 
		sizeof("QueryFeedBacksCountResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBacksCount_struct, QueryFeedBacksCountResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryFeedBacksCount_map =
{
	0x88868846,
	"QueryFeedBacksCount",
	L"QueryFeedBacksCountResponse",
	sizeof("QueryFeedBacksCount")-1,
	sizeof("QueryFeedBacksCountResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryFeedBacksCount_entries,
	sizeof(__CXMServer_QueryFeedBacksCount_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryInsideUsers_struct
{
	int sessionId;
	User *QueryInsideUsersResult;
	int __QueryInsideUsersResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryInsideUsers_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryInsideUsers_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xDD2E12E3, 
		"QueryInsideUsersResult", 
		L"QueryInsideUsersResult", 
		sizeof("QueryInsideUsersResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryInsideUsers_struct, QueryInsideUsersResult),
		NULL,
		&__User_map,
		1+1,
	},
	{
		0xDD2E12E3,
		"__QueryInsideUsersResult_nSizeIs",
		L"__QueryInsideUsersResult_nSizeIs",
		sizeof("__QueryInsideUsersResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryInsideUsers_struct, __QueryInsideUsersResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryInsideUsers_map =
{
	0xE0A577F3,
	"QueryInsideUsers",
	L"QueryInsideUsersResponse",
	sizeof("QueryInsideUsers")-1,
	sizeof("QueryInsideUsersResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryInsideUsers_entries,
	sizeof(__CXMServer_QueryInsideUsers_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_GetLoginStruct_struct
{
	LoginStruct GetLoginStructResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_GetLoginStruct_entries[] =
{

	{
		0xA0B7581D, 
		"GetLoginStructResult", 
		L"GetLoginStructResult", 
		sizeof("GetLoginStructResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_GetLoginStruct_struct, GetLoginStructResult),
		NULL,
		&__LoginStruct_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_GetLoginStruct_map =
{
	0xAB94F3AD,
	"GetLoginStruct",
	L"GetLoginStructResponse",
	sizeof("GetLoginStruct")-1,
	sizeof("GetLoginStructResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_GetLoginStruct_entries,
	sizeof(__CXMServer_GetLoginStruct_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryTestItemArr_struct
{
	int sessionId;
	TestItem *QueryTestItemArrResult;
	int __QueryTestItemArrResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryTestItemArr_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemArr_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x58BD3269, 
		"QueryTestItemArrResult", 
		L"QueryTestItemArrResult", 
		sizeof("QueryTestItemArrResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryTestItemArr_struct, QueryTestItemArrResult),
		NULL,
		&__TestItem_map,
		1+1,
	},
	{
		0x58BD3269,
		"__QueryTestItemArrResult_nSizeIs",
		L"__QueryTestItemArrResult_nSizeIs",
		sizeof("__QueryTestItemArrResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryTestItemArr_struct, __QueryTestItemArrResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryTestItemArr_map =
{
	0x7C7A90F9,
	"QueryTestItemArr",
	L"QueryTestItemArrResponse",
	sizeof("QueryTestItemArr")-1,
	sizeof("QueryTestItemArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryTestItemArr_entries,
	sizeof(__CXMServer_QueryTestItemArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteNotification_struct
{
	int sessionId;
	int id;
	bool DeleteNotificationResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteNotification_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteNotification_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteNotification_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xD3DE5E19, 
		"DeleteNotificationResult", 
		L"DeleteNotificationResult", 
		sizeof("DeleteNotificationResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteNotification_struct, DeleteNotificationResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteNotification_map =
{
	0x449568A9,
	"DeleteNotification",
	L"DeleteNotificationResponse",
	sizeof("DeleteNotification")-1,
	sizeof("DeleteNotificationResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteNotification_entries,
	sizeof(__CXMServer_DeleteNotification_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateNews_struct
{
	int sessionId;
	News news;
	int SaveOrUpdateNewsResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateNews_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateNews_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x003E0F1D, 
		"news", 
		L"news", 
		sizeof("news")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateNews_struct, news),
		NULL,
		&__News_map,
		-1,
	},
	{
		0x80B4546F, 
		"SaveOrUpdateNewsResult", 
		L"SaveOrUpdateNewsResult", 
		sizeof("SaveOrUpdateNewsResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateNews_struct, SaveOrUpdateNewsResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateNews_map =
{
	0x7EC24C7F,
	"SaveOrUpdateNews",
	L"SaveOrUpdateNewsResponse",
	sizeof("SaveOrUpdateNews")-1,
	sizeof("SaveOrUpdateNewsResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateNews_entries,
	sizeof(__CXMServer_SaveOrUpdateNews_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryVersionRevisionArrByProductionId_struct
{
	int sessionId;
	int productionId;
	VersionRevision *QueryVersionRevisionArrByProductionIdResult;
	int __QueryVersionRevisionArrByProductionIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryVersionRevisionArrByProductionId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryVersionRevisionArrByProductionId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xE40F9E34, 
		"productionId", 
		L"productionId", 
		sizeof("productionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryVersionRevisionArrByProductionId_struct, productionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x5D5778FE, 
		"QueryVersionRevisionArrByProductionIdResult", 
		L"QueryVersionRevisionArrByProductionIdResult", 
		sizeof("QueryVersionRevisionArrByProductionIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryVersionRevisionArrByProductionId_struct, QueryVersionRevisionArrByProductionIdResult),
		NULL,
		&__VersionRevision_map,
		2+1,
	},
	{
		0x5D5778FE,
		"__QueryVersionRevisionArrByProductionIdResult_nSizeIs",
		L"__QueryVersionRevisionArrByProductionIdResult_nSizeIs",
		sizeof("__QueryVersionRevisionArrByProductionIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryVersionRevisionArrByProductionId_struct, __QueryVersionRevisionArrByProductionIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryVersionRevisionArrByProductionId_map =
{
	0x10C0D0CE,
	"QueryVersionRevisionArrByProductionId",
	L"QueryVersionRevisionArrByProductionIdResponse",
	sizeof("QueryVersionRevisionArrByProductionId")-1,
	sizeof("QueryVersionRevisionArrByProductionIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryVersionRevisionArrByProductionId_entries,
	sizeof(__CXMServer_QueryVersionRevisionArrByProductionId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryFeedBackArrByProductionId_struct
{
	int sessionId;
	int productionId;
	FeedBack *QueryFeedBackArrByProductionIdResult;
	int __QueryFeedBackArrByProductionIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryFeedBackArrByProductionId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrByProductionId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xE40F9E34, 
		"productionId", 
		L"productionId", 
		sizeof("productionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrByProductionId_struct, productionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xA11FB10E, 
		"QueryFeedBackArrByProductionIdResult", 
		L"QueryFeedBackArrByProductionIdResult", 
		sizeof("QueryFeedBackArrByProductionIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBackArrByProductionId_struct, QueryFeedBackArrByProductionIdResult),
		NULL,
		&__FeedBack_map,
		2+1,
	},
	{
		0xA11FB10E,
		"__QueryFeedBackArrByProductionIdResult_nSizeIs",
		L"__QueryFeedBackArrByProductionIdResult_nSizeIs",
		sizeof("__QueryFeedBackArrByProductionIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryFeedBackArrByProductionId_struct, __QueryFeedBackArrByProductionIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryFeedBackArrByProductionId_map =
{
	0x67774CDE,
	"QueryFeedBackArrByProductionId",
	L"QueryFeedBackArrByProductionIdResponse",
	sizeof("QueryFeedBackArrByProductionId")-1,
	sizeof("QueryFeedBackArrByProductionIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryFeedBackArrByProductionId_entries,
	sizeof(__CXMServer_QueryFeedBackArrByProductionId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteDeveLop_testItemByDevelopLopId_struct
{
	int sessionId;
	int developLogId;
	bool DeleteDeveLop_testItemByDevelopLopIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteDeveLop_testItemByDevelopLopId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteDeveLop_testItemByDevelopLopId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xA5C1487E, 
		"developLogId", 
		L"developLogId", 
		sizeof("developLogId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteDeveLop_testItemByDevelopLopId_struct, developLogId),
		NULL,
		NULL,
		-1,
	},
	{
		0x87165B91, 
		"DeleteDeveLop_testItemByDevelopLopIdResult", 
		L"DeleteDeveLop_testItemByDevelopLopIdResult", 
		sizeof("DeleteDeveLop_testItemByDevelopLopIdResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteDeveLop_testItemByDevelopLopId_struct, DeleteDeveLop_testItemByDevelopLopIdResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteDeveLop_testItemByDevelopLopId_map =
{
	0xA5C2A421,
	"DeleteDeveLop_testItemByDevelopLopId",
	L"DeleteDeveLop_testItemByDevelopLopIdResponse",
	sizeof("DeleteDeveLop_testItemByDevelopLopId")-1,
	sizeof("DeleteDeveLop_testItemByDevelopLopIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteDeveLop_testItemByDevelopLopId_entries,
	sizeof(__CXMServer_DeleteDeveLop_testItemByDevelopLopId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteClient_DownloadItemByClient_struct
{
	int clientId;
	bool DeleteClient_DownloadItemByClientResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteClient_DownloadItemByClient_entries[] =
{

	{
		0xD356924C, 
		"clientId", 
		L"clientId", 
		sizeof("clientId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteClient_DownloadItemByClient_struct, clientId),
		NULL,
		NULL,
		-1,
	},
	{
		0x91C50651, 
		"DeleteClient_DownloadItemByClientResult", 
		L"DeleteClient_DownloadItemByClientResult", 
		sizeof("DeleteClient_DownloadItemByClientResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteClient_DownloadItemByClient_struct, DeleteClient_DownloadItemByClientResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteClient_DownloadItemByClient_map =
{
	0x16C6FEE1,
	"DeleteClient_DownloadItemByClient",
	L"DeleteClient_DownloadItemByClientResponse",
	sizeof("DeleteClient_DownloadItemByClient")-1,
	sizeof("DeleteClient_DownloadItemByClientResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteClient_DownloadItemByClient_entries,
	sizeof(__CXMServer_DeleteClient_DownloadItemByClient_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryAttendanceByDate_struct
{
	int sessionId;
	BSTR startDate;
	BSTR endDate;
	Attendance *QueryAttendanceByDateResult;
	int __QueryAttendanceByDateResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryAttendanceByDate_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryAttendanceByDate_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x9ED2666C, 
		"startDate", 
		L"startDate", 
		sizeof("startDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryAttendanceByDate_struct, startDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x6689F575, 
		"endDate", 
		L"endDate", 
		sizeof("endDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryAttendanceByDate_struct, endDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x3D278B65, 
		"QueryAttendanceByDateResult", 
		L"QueryAttendanceByDateResult", 
		sizeof("QueryAttendanceByDateResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryAttendanceByDate_struct, QueryAttendanceByDateResult),
		NULL,
		&__Attendance_map,
		3+1,
	},
	{
		0x3D278B65,
		"__QueryAttendanceByDateResult_nSizeIs",
		L"__QueryAttendanceByDateResult_nSizeIs",
		sizeof("__QueryAttendanceByDateResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryAttendanceByDate_struct, __QueryAttendanceByDateResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryAttendanceByDate_map =
{
	0x24DF18F5,
	"QueryAttendanceByDate",
	L"QueryAttendanceByDateResponse",
	sizeof("QueryAttendanceByDate")-1,
	sizeof("QueryAttendanceByDateResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryAttendanceByDate_entries,
	sizeof(__CXMServer_QueryAttendanceByDate_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_GetTaskDispatch_struct
{
	TaskDispatch GetTaskDispatchResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_GetTaskDispatch_entries[] =
{

	{
		0x02AC39A2, 
		"GetTaskDispatchResult", 
		L"GetTaskDispatchResult", 
		sizeof("GetTaskDispatchResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_GetTaskDispatch_struct, GetTaskDispatchResult),
		NULL,
		&__TaskDispatch_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_GetTaskDispatch_map =
{
	0x5E484A72,
	"GetTaskDispatch",
	L"GetTaskDispatchResponse",
	sizeof("GetTaskDispatch")-1,
	sizeof("GetTaskDispatchResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_GetTaskDispatch_entries,
	sizeof(__CXMServer_GetTaskDispatch_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteRole_struct
{
	int sessionId;
	int id;
	bool DeleteRoleResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteRole_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteRole_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteRole_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x63404084, 
		"DeleteRoleResult", 
		L"DeleteRoleResult", 
		sizeof("DeleteRoleResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteRole_struct, DeleteRoleResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteRole_map =
{
	0x33F991D4,
	"DeleteRole",
	L"DeleteRoleResponse",
	sizeof("DeleteRole")-1,
	sizeof("DeleteRoleResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteRole_entries,
	sizeof(__CXMServer_DeleteRole_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_UpdateVersion_TestItemArr_struct
{
	int sessionId;
	Version_testItem *vtArr;
	int __vtArr_nSizeIs;
	bool UpdateVersion_TestItemArrResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_UpdateVersion_TestItemArr_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateVersion_TestItemArr_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x08980B0F, 
		"vtArr", 
		L"vtArr", 
		sizeof("vtArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_UpdateVersion_TestItemArr_struct, vtArr),
		NULL,
		&__Version_testItem_map,
		1+1,
	},
	{
		0x08980B0F,
		"__vtArr_nSizeIs",
		L"__vtArr_nSizeIs",
		sizeof("__vtArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_UpdateVersion_TestItemArr_struct, __vtArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x250EA15B, 
		"UpdateVersion_TestItemArrResult", 
		L"UpdateVersion_TestItemArrResult", 
		sizeof("UpdateVersion_TestItemArrResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateVersion_TestItemArr_struct, UpdateVersion_TestItemArrResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_UpdateVersion_TestItemArr_map =
{
	0xA2E3846B,
	"UpdateVersion_TestItemArr",
	L"UpdateVersion_TestItemArrResponse",
	sizeof("UpdateVersion_TestItemArr")-1,
	sizeof("UpdateVersion_TestItemArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_UpdateVersion_TestItemArr_entries,
	sizeof(__CXMServer_UpdateVersion_TestItemArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateClient_struct
{
	int sessionId;
	Client client;
	bool isProduct;
	bool isDownloadItem;
	int SaveOrUpdateClientResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateClient_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateClient_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xEECC5CBF, 
		"client", 
		L"client", 
		sizeof("client")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateClient_struct, client),
		NULL,
		&__Client_map,
		-1,
	},
	{
		0x08CD833D, 
		"isProduct", 
		L"isProduct", 
		sizeof("isProduct")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateClient_struct, isProduct),
		NULL,
		NULL,
		-1,
	},
	{
		0xAD087243, 
		"isDownloadItem", 
		L"isDownloadItem", 
		sizeof("isDownloadItem")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateClient_struct, isDownloadItem),
		NULL,
		NULL,
		-1,
	},
	{
		0x7E91FC51, 
		"SaveOrUpdateClientResult", 
		L"SaveOrUpdateClientResult", 
		sizeof("SaveOrUpdateClientResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateClient_struct, SaveOrUpdateClientResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateClient_map =
{
	0x6AA974E1,
	"SaveOrUpdateClient",
	L"SaveOrUpdateClientResponse",
	sizeof("SaveOrUpdateClient")-1,
	sizeof("SaveOrUpdateClientResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateClient_entries,
	sizeof(__CXMServer_SaveOrUpdateClient_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryAllDevelopLog_struct
{
	int sessionId;
	DevelopLog *QueryAllDevelopLogResult;
	int __QueryAllDevelopLogResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryAllDevelopLog_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryAllDevelopLog_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x6615FC3F, 
		"QueryAllDevelopLogResult", 
		L"QueryAllDevelopLogResult", 
		sizeof("QueryAllDevelopLogResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryAllDevelopLog_struct, QueryAllDevelopLogResult),
		NULL,
		&__DevelopLog_map,
		1+1,
	},
	{
		0x6615FC3F,
		"__QueryAllDevelopLogResult_nSizeIs",
		L"__QueryAllDevelopLogResult_nSizeIs",
		sizeof("__QueryAllDevelopLogResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryAllDevelopLog_struct, __QueryAllDevelopLogResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryAllDevelopLog_map =
{
	0x432D284F,
	"QueryAllDevelopLog",
	L"QueryAllDevelopLogResponse",
	sizeof("QueryAllDevelopLog")-1,
	sizeof("QueryAllDevelopLogResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryAllDevelopLog_entries,
	sizeof(__CXMServer_QueryAllDevelopLog_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_UpdateFeedBacksSerialNo_struct
{
	int sessionId;
	int *idArr;
	int __idArr_nSizeIs;
	int *serialArr;
	int __serialArr_nSizeIs;
	bool UpdateFeedBacksSerialNoResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_UpdateFeedBacksSerialNo_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateFeedBacksSerialNo_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x07A40672, 
		"idArr", 
		L"idArr", 
		sizeof("idArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_UpdateFeedBacksSerialNo_struct, idArr),
		NULL,
		NULL,
		1+1,
	},
	{
		0x07A40672,
		"__idArr_nSizeIs",
		L"__idArr_nSizeIs",
		sizeof("__idArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_UpdateFeedBacksSerialNo_struct, __idArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xCD541DA5, 
		"serialArr", 
		L"serialArr", 
		sizeof("serialArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_UpdateFeedBacksSerialNo_struct, serialArr),
		NULL,
		NULL,
		3+1,
	},
	{
		0xCD541DA5,
		"__serialArr_nSizeIs",
		L"__serialArr_nSizeIs",
		sizeof("__serialArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_UpdateFeedBacksSerialNo_struct, __serialArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xE7DCFE17, 
		"UpdateFeedBacksSerialNoResult", 
		L"UpdateFeedBacksSerialNoResult", 
		sizeof("UpdateFeedBacksSerialNoResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateFeedBacksSerialNo_struct, UpdateFeedBacksSerialNoResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_UpdateFeedBacksSerialNo_map =
{
	0x52BC0027,
	"UpdateFeedBacksSerialNo",
	L"UpdateFeedBacksSerialNoResponse",
	sizeof("UpdateFeedBacksSerialNo")-1,
	sizeof("UpdateFeedBacksSerialNoResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_UpdateFeedBacksSerialNo_entries,
	sizeof(__CXMServer_UpdateFeedBacksSerialNo_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownloadFileHeader_struct
{
	int sessionId;
	int id;
	FileHeader DownloadFileHeaderResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownloadFileHeader_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadFileHeader_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadFileHeader_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x96C8FBE0, 
		"DownloadFileHeaderResult", 
		L"DownloadFileHeaderResult", 
		sizeof("DownloadFileHeaderResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadFileHeader_struct, DownloadFileHeaderResult),
		NULL,
		&__FileHeader_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownloadFileHeader_map =
{
	0x6C9E9430,
	"DownloadFileHeader",
	L"DownloadFileHeaderResponse",
	sizeof("DownloadFileHeader")-1,
	sizeof("DownloadFileHeaderResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownloadFileHeader_entries,
	sizeof(__CXMServer_DownloadFileHeader_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryLikeClients_struct
{
	int sessionId;
	BSTR keyword;
	bool isProduct;
	bool isDownloadItem;
	Client *QueryLikeClientsResult;
	int __QueryLikeClientsResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryLikeClients_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryLikeClients_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x21012985, 
		"keyword", 
		L"keyword", 
		sizeof("keyword")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryLikeClients_struct, keyword),
		NULL,
		NULL,
		-1,
	},
	{
		0x08CD833D, 
		"isProduct", 
		L"isProduct", 
		sizeof("isProduct")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryLikeClients_struct, isProduct),
		NULL,
		NULL,
		-1,
	},
	{
		0xAD087243, 
		"isDownloadItem", 
		L"isDownloadItem", 
		sizeof("isDownloadItem")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryLikeClients_struct, isDownloadItem),
		NULL,
		NULL,
		-1,
	},
	{
		0x906AD70C, 
		"QueryLikeClientsResult", 
		L"QueryLikeClientsResult", 
		sizeof("QueryLikeClientsResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryLikeClients_struct, QueryLikeClientsResult),
		NULL,
		&__Client_map,
		4+1,
	},
	{
		0x906AD70C,
		"__QueryLikeClientsResult_nSizeIs",
		L"__QueryLikeClientsResult_nSizeIs",
		sizeof("__QueryLikeClientsResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryLikeClients_struct, __QueryLikeClientsResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryLikeClients_map =
{
	0x5623EA5C,
	"QueryLikeClients",
	L"QueryLikeClientsResponse",
	sizeof("QueryLikeClients")-1,
	sizeof("QueryLikeClientsResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryLikeClients_entries,
	sizeof(__CXMServer_QueryLikeClients_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryWorkLogByDateAndProId_struct
{
	int sessionId;
	BSTR startDate;
	BSTR endDate;
	int proId;
	WorkLog *QueryWorkLogByDateAndProIdResult;
	int __QueryWorkLogByDateAndProIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryWorkLogByDateAndProId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryWorkLogByDateAndProId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x9ED2666C, 
		"startDate", 
		L"startDate", 
		sizeof("startDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryWorkLogByDateAndProId_struct, startDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x6689F575, 
		"endDate", 
		L"endDate", 
		sizeof("endDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryWorkLogByDateAndProId_struct, endDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x082B1D9E, 
		"proId", 
		L"proId", 
		sizeof("proId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryWorkLogByDateAndProId_struct, proId),
		NULL,
		NULL,
		-1,
	},
	{
		0xCF208364, 
		"QueryWorkLogByDateAndProIdResult", 
		L"QueryWorkLogByDateAndProIdResult", 
		sizeof("QueryWorkLogByDateAndProIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryWorkLogByDateAndProId_struct, QueryWorkLogByDateAndProIdResult),
		NULL,
		&__WorkLog_map,
		4+1,
	},
	{
		0xCF208364,
		"__QueryWorkLogByDateAndProIdResult_nSizeIs",
		L"__QueryWorkLogByDateAndProIdResult_nSizeIs",
		sizeof("__QueryWorkLogByDateAndProIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryWorkLogByDateAndProId_struct, __QueryWorkLogByDateAndProIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryWorkLogByDateAndProId_map =
{
	0x18F60CB4,
	"QueryWorkLogByDateAndProId",
	L"QueryWorkLogByDateAndProIdResponse",
	sizeof("QueryWorkLogByDateAndProId")-1,
	sizeof("QueryWorkLogByDateAndProIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryWorkLogByDateAndProId_entries,
	sizeof(__CXMServer_QueryWorkLogByDateAndProId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownloadPartBuildById_struct
{
	int id;
	PartBuild DownloadPartBuildByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownloadPartBuildById_entries[] =
{

	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadPartBuildById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x5A7E4E26, 
		"DownloadPartBuildByIdResult", 
		L"DownloadPartBuildByIdResult", 
		sizeof("DownloadPartBuildByIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadPartBuildById_struct, DownloadPartBuildByIdResult),
		NULL,
		&__PartBuild_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownloadPartBuildById_map =
{
	0xF2F18FF6,
	"DownloadPartBuildById",
	L"DownloadPartBuildByIdResponse",
	sizeof("DownloadPartBuildById")-1,
	sizeof("DownloadPartBuildByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownloadPartBuildById_entries,
	sizeof(__CXMServer_DownloadPartBuildById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_LogoutUser_struct
{
	int sessionId;
	bool LogoutUserResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_LogoutUser_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_LogoutUser_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x6984AF38, 
		"LogoutUserResult", 
		L"LogoutUserResult", 
		sizeof("LogoutUserResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_LogoutUser_struct, LogoutUserResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_LogoutUser_map =
{
	0xDD147D88,
	"LogoutUser",
	L"LogoutUserResponse",
	sizeof("LogoutUser")-1,
	sizeof("LogoutUserResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_LogoutUser_entries,
	sizeof(__CXMServer_LogoutUser_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryLastReplyByFeedBackId_struct
{
	int sessionId;
	int feedBackId;
	Reply QueryLastReplyByFeedBackIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryLastReplyByFeedBackId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryLastReplyByFeedBackId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x7A0EFD92, 
		"feedBackId", 
		L"feedBackId", 
		sizeof("feedBackId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryLastReplyByFeedBackId_struct, feedBackId),
		NULL,
		NULL,
		-1,
	},
	{
		0x2408A7E2, 
		"QueryLastReplyByFeedBackIdResult", 
		L"QueryLastReplyByFeedBackIdResult", 
		sizeof("QueryLastReplyByFeedBackIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryLastReplyByFeedBackId_struct, QueryLastReplyByFeedBackIdResult),
		NULL,
		&__Reply_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryLastReplyByFeedBackId_map =
{
	0x487948B2,
	"QueryLastReplyByFeedBackId",
	L"QueryLastReplyByFeedBackIdResponse",
	sizeof("QueryLastReplyByFeedBackId")-1,
	sizeof("QueryLastReplyByFeedBackIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryLastReplyByFeedBackId_entries,
	sizeof(__CXMServer_QueryLastReplyByFeedBackId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownLoadClientById_struct
{
	int sessionId;
	int id;
	Client DownLoadClientByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownLoadClientById_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadClientById_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadClientById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x3DF2EC9E, 
		"DownLoadClientByIdResult", 
		L"DownLoadClientByIdResult", 
		sizeof("DownLoadClientByIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadClientById_struct, DownLoadClientByIdResult),
		NULL,
		&__Client_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownLoadClientById_map =
{
	0x8607AC6E,
	"DownLoadClientById",
	L"DownLoadClientByIdResponse",
	sizeof("DownLoadClientById")-1,
	sizeof("DownLoadClientByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownLoadClientById_entries,
	sizeof(__CXMServer_DownLoadClientById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryGroupFeedBacks_struct
{
	int sessionId;
	int superiorId;
	FeedBack *QueryGroupFeedBacksResult;
	int __QueryGroupFeedBacksResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryGroupFeedBacks_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryGroupFeedBacks_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xDABCE346, 
		"superiorId", 
		L"superiorId", 
		sizeof("superiorId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryGroupFeedBacks_struct, superiorId),
		NULL,
		NULL,
		-1,
	},
	{
		0xF804539A, 
		"QueryGroupFeedBacksResult", 
		L"QueryGroupFeedBacksResult", 
		sizeof("QueryGroupFeedBacksResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryGroupFeedBacks_struct, QueryGroupFeedBacksResult),
		NULL,
		&__FeedBack_map,
		2+1,
	},
	{
		0xF804539A,
		"__QueryGroupFeedBacksResult_nSizeIs",
		L"__QueryGroupFeedBacksResult_nSizeIs",
		sizeof("__QueryGroupFeedBacksResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryGroupFeedBacks_struct, __QueryGroupFeedBacksResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryGroupFeedBacks_map =
{
	0x0A0EC26A,
	"QueryGroupFeedBacks",
	L"QueryGroupFeedBacksResponse",
	sizeof("QueryGroupFeedBacks")-1,
	sizeof("QueryGroupFeedBacksResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryGroupFeedBacks_entries,
	sizeof(__CXMServer_QueryGroupFeedBacks_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateFeedbackGroup_struct
{
	int sessionId;
	FeedBack *feedbackArr;
	int __feedbackArr_nSizeIs;
	int *SaveOrUpdateFeedbackGroupResult;
	int __SaveOrUpdateFeedbackGroupResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateFeedbackGroup_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateFeedbackGroup_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x5B36282A, 
		"feedbackArr", 
		L"feedbackArr", 
		sizeof("feedbackArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_SaveOrUpdateFeedbackGroup_struct, feedbackArr),
		NULL,
		&__FeedBack_map,
		1+1,
	},
	{
		0x5B36282A,
		"__feedbackArr_nSizeIs",
		L"__feedbackArr_nSizeIs",
		sizeof("__feedbackArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_SaveOrUpdateFeedbackGroup_struct, __feedbackArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x3BDF72E4, 
		"SaveOrUpdateFeedbackGroupResult", 
		L"SaveOrUpdateFeedbackGroupResult", 
		sizeof("SaveOrUpdateFeedbackGroupResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_SaveOrUpdateFeedbackGroup_struct, SaveOrUpdateFeedbackGroupResult),
		NULL,
		NULL,
		3+1,
	},
	{
		0x3BDF72E4,
		"__SaveOrUpdateFeedbackGroupResult_nSizeIs",
		L"__SaveOrUpdateFeedbackGroupResult_nSizeIs",
		sizeof("__SaveOrUpdateFeedbackGroupResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_SaveOrUpdateFeedbackGroup_struct, __SaveOrUpdateFeedbackGroupResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateFeedbackGroup_map =
{
	0xB12EDC34,
	"SaveOrUpdateFeedbackGroup",
	L"SaveOrUpdateFeedbackGroupResponse",
	sizeof("SaveOrUpdateFeedbackGroup")-1,
	sizeof("SaveOrUpdateFeedbackGroupResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateFeedbackGroup_entries,
	sizeof(__CXMServer_SaveOrUpdateFeedbackGroup_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryUserByRoleId_struct
{
	int sessionId;
	int roleId;
	User *QueryUserByRoleIdResult;
	int __QueryUserByRoleIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryUserByRoleId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryUserByRoleId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x1201A7FF, 
		"roleId", 
		L"roleId", 
		sizeof("roleId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryUserByRoleId_struct, roleId),
		NULL,
		NULL,
		-1,
	},
	{
		0xF4E1AB4E, 
		"QueryUserByRoleIdResult", 
		L"QueryUserByRoleIdResult", 
		sizeof("QueryUserByRoleIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryUserByRoleId_struct, QueryUserByRoleIdResult),
		NULL,
		&__User_map,
		2+1,
	},
	{
		0xF4E1AB4E,
		"__QueryUserByRoleIdResult_nSizeIs",
		L"__QueryUserByRoleIdResult_nSizeIs",
		sizeof("__QueryUserByRoleIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryUserByRoleId_struct, __QueryUserByRoleIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryUserByRoleId_map =
{
	0xB3A0D71E,
	"QueryUserByRoleId",
	L"QueryUserByRoleIdResponse",
	sizeof("QueryUserByRoleId")-1,
	sizeof("QueryUserByRoleIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryUserByRoleId_entries,
	sizeof(__CXMServer_QueryUserByRoleId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteClient_ProductionByProduct_struct
{
	int productId;
	bool DeleteClient_ProductionByProductResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteClient_ProductionByProduct_entries[] =
{

	{
		0x7E03E6EE, 
		"productId", 
		L"productId", 
		sizeof("productId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteClient_ProductionByProduct_struct, productId),
		NULL,
		NULL,
		-1,
	},
	{
		0x7E94AA53, 
		"DeleteClient_ProductionByProductResult", 
		L"DeleteClient_ProductionByProductResult", 
		sizeof("DeleteClient_ProductionByProductResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteClient_ProductionByProduct_struct, DeleteClient_ProductionByProductResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteClient_ProductionByProduct_map =
{
	0x760FAB63,
	"DeleteClient_ProductionByProduct",
	L"DeleteClient_ProductionByProductResponse",
	sizeof("DeleteClient_ProductionByProduct")-1,
	sizeof("DeleteClient_ProductionByProductResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteClient_ProductionByProduct_entries,
	sizeof(__CXMServer_DeleteClient_ProductionByProduct_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateFeedBack_struct
{
	int sessionId;
	FeedBack feedBack;
	bool isFile;
	bool isRevert;
	int SaveOrUpdateFeedBackResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateFeedBack_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateFeedBack_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x73C53E85, 
		"feedBack", 
		L"feedBack", 
		sizeof("feedBack")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateFeedBack_struct, feedBack),
		NULL,
		&__FeedBack_map,
		-1,
	},
	{
		0xFD36DB7C, 
		"isFile", 
		L"isFile", 
		sizeof("isFile")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateFeedBack_struct, isFile),
		NULL,
		NULL,
		-1,
	},
	{
		0x4216BAF4, 
		"isRevert", 
		L"isRevert", 
		sizeof("isRevert")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateFeedBack_struct, isRevert),
		NULL,
		NULL,
		-1,
	},
	{
		0x8116F357, 
		"SaveOrUpdateFeedBackResult", 
		L"SaveOrUpdateFeedBackResult", 
		sizeof("SaveOrUpdateFeedBackResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateFeedBack_struct, SaveOrUpdateFeedBackResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateFeedBack_map =
{
	0x22484567,
	"SaveOrUpdateFeedBack",
	L"SaveOrUpdateFeedBackResponse",
	sizeof("SaveOrUpdateFeedBack")-1,
	sizeof("SaveOrUpdateFeedBackResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateFeedBack_entries,
	sizeof(__CXMServer_SaveOrUpdateFeedBack_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteUser_RoleByUserIdAndRoleId_struct
{
	int sessionId;
	int userId;
	int roleId;
	bool DeleteUser_RoleByUserIdAndRoleIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteUser_RoleByUserIdAndRoleId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteUser_RoleByUserIdAndRoleId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x1945E48C, 
		"userId", 
		L"userId", 
		sizeof("userId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteUser_RoleByUserIdAndRoleId_struct, userId),
		NULL,
		NULL,
		-1,
	},
	{
		0x1201A7FF, 
		"roleId", 
		L"roleId", 
		sizeof("roleId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteUser_RoleByUserIdAndRoleId_struct, roleId),
		NULL,
		NULL,
		-1,
	},
	{
		0x3AB050DB, 
		"DeleteUser_RoleByUserIdAndRoleIdResult", 
		L"DeleteUser_RoleByUserIdAndRoleIdResult", 
		sizeof("DeleteUser_RoleByUserIdAndRoleIdResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteUser_RoleByUserIdAndRoleId_struct, DeleteUser_RoleByUserIdAndRoleIdResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteUser_RoleByUserIdAndRoleId_map =
{
	0xA7AF13EB,
	"DeleteUser_RoleByUserIdAndRoleId",
	L"DeleteUser_RoleByUserIdAndRoleIdResponse",
	sizeof("DeleteUser_RoleByUserIdAndRoleId")-1,
	sizeof("DeleteUser_RoleByUserIdAndRoleIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteUser_RoleByUserIdAndRoleId_entries,
	sizeof(__CXMServer_DeleteUser_RoleByUserIdAndRoleId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateTestItemSet_struct
{
	int sessionId;
	TestItemSet testSet;
	int *tiIdArr;
	int __tiIdArr_nSizeIs;
	int SaveOrUpdateTestItemSetResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateTestItemSet_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateTestItemSet_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xD55E9B6C, 
		"testSet", 
		L"testSet", 
		sizeof("testSet")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateTestItemSet_struct, testSet),
		NULL,
		&__TestItemSet_map,
		-1,
	},
	{
		0xDBB227CF, 
		"tiIdArr", 
		L"tiIdArr", 
		sizeof("tiIdArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_SaveOrUpdateTestItemSet_struct, tiIdArr),
		NULL,
		NULL,
		2+1,
	},
	{
		0xDBB227CF,
		"__tiIdArr_nSizeIs",
		L"__tiIdArr_nSizeIs",
		sizeof("__tiIdArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_SaveOrUpdateTestItemSet_struct, __tiIdArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x7068640D, 
		"SaveOrUpdateTestItemSetResult", 
		L"SaveOrUpdateTestItemSetResult", 
		sizeof("SaveOrUpdateTestItemSetResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateTestItemSet_struct, SaveOrUpdateTestItemSetResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateTestItemSet_map =
{
	0x2BB8BB9D,
	"SaveOrUpdateTestItemSet",
	L"SaveOrUpdateTestItemSetResponse",
	sizeof("SaveOrUpdateTestItemSet")-1,
	sizeof("SaveOrUpdateTestItemSetResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateTestItemSet_entries,
	sizeof(__CXMServer_SaveOrUpdateTestItemSet_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_ClearUserPassword_struct
{
	int userId;
	bool ClearUserPasswordResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_ClearUserPassword_entries[] =
{

	{
		0x1945E48C, 
		"userId", 
		L"userId", 
		sizeof("userId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_ClearUserPassword_struct, userId),
		NULL,
		NULL,
		-1,
	},
	{
		0x56995C98, 
		"ClearUserPasswordResult", 
		L"ClearUserPasswordResult", 
		sizeof("ClearUserPasswordResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_ClearUserPassword_struct, ClearUserPasswordResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_ClearUserPassword_map =
{
	0x620A02E8,
	"ClearUserPassword",
	L"ClearUserPasswordResponse",
	sizeof("ClearUserPassword")-1,
	sizeof("ClearUserPasswordResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_ClearUserPassword_entries,
	sizeof(__CXMServer_ClearUserPassword_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryAllAttendance_struct
{
	Attendance *QueryAllAttendanceResult;
	int __QueryAllAttendanceResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryAllAttendance_entries[] =
{

	{
		0x91EE83C5, 
		"QueryAllAttendanceResult", 
		L"QueryAllAttendanceResult", 
		sizeof("QueryAllAttendanceResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryAllAttendance_struct, QueryAllAttendanceResult),
		NULL,
		&__Attendance_map,
		0+1,
	},
	{
		0x91EE83C5,
		"__QueryAllAttendanceResult_nSizeIs",
		L"__QueryAllAttendanceResult_nSizeIs",
		sizeof("__QueryAllAttendanceResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryAllAttendance_struct, __QueryAllAttendanceResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryAllAttendance_map =
{
	0xC745A955,
	"QueryAllAttendance",
	L"QueryAllAttendanceResponse",
	sizeof("QueryAllAttendance")-1,
	sizeof("QueryAllAttendanceResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryAllAttendance_entries,
	sizeof(__CXMServer_QueryAllAttendance_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteDeveLop_testItemByItemId_struct
{
	int sessionId;
	int itemId;
	bool DeleteDeveLop_testItemByItemIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteDeveLop_testItemByItemId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteDeveLop_testItemByItemId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xFD5A003C, 
		"itemId", 
		L"itemId", 
		sizeof("itemId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteDeveLop_testItemByItemId_struct, itemId),
		NULL,
		NULL,
		-1,
	},
	{
		0xE9D07C86, 
		"DeleteDeveLop_testItemByItemIdResult", 
		L"DeleteDeveLop_testItemByItemIdResult", 
		sizeof("DeleteDeveLop_testItemByItemIdResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteDeveLop_testItemByItemId_struct, DeleteDeveLop_testItemByItemIdResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteDeveLop_testItemByItemId_map =
{
	0x9F88D656,
	"DeleteDeveLop_testItemByItemId",
	L"DeleteDeveLop_testItemByItemIdResponse",
	sizeof("DeleteDeveLop_testItemByItemId")-1,
	sizeof("DeleteDeveLop_testItemByItemIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteDeveLop_testItemByItemId_entries,
	sizeof(__CXMServer_DeleteDeveLop_testItemByItemId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryWorkLogByUserId_struct
{
	int sessionId;
	int userId;
	WorkLog *QueryWorkLogByUserIdResult;
	int __QueryWorkLogByUserIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryWorkLogByUserId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryWorkLogByUserId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x1945E48C, 
		"userId", 
		L"userId", 
		sizeof("userId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryWorkLogByUserId_struct, userId),
		NULL,
		NULL,
		-1,
	},
	{
		0x1B98BCE1, 
		"QueryWorkLogByUserIdResult", 
		L"QueryWorkLogByUserIdResult", 
		sizeof("QueryWorkLogByUserIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryWorkLogByUserId_struct, QueryWorkLogByUserIdResult),
		NULL,
		&__WorkLog_map,
		2+1,
	},
	{
		0x1B98BCE1,
		"__QueryWorkLogByUserIdResult_nSizeIs",
		L"__QueryWorkLogByUserIdResult_nSizeIs",
		sizeof("__QueryWorkLogByUserIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryWorkLogByUserId_struct, __QueryWorkLogByUserIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryWorkLogByUserId_map =
{
	0x64629971,
	"QueryWorkLogByUserId",
	L"QueryWorkLogByUserIdResponse",
	sizeof("QueryWorkLogByUserId")-1,
	sizeof("QueryWorkLogByUserIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryWorkLogByUserId_entries,
	sizeof(__CXMServer_QueryWorkLogByUserId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryFeedBackArrBySubmitDate_struct
{
	int sessionId;
	BSTR startDate;
	BSTR endDate;
	FeedBack *QueryFeedBackArrBySubmitDateResult;
	int __QueryFeedBackArrBySubmitDateResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryFeedBackArrBySubmitDate_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrBySubmitDate_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x9ED2666C, 
		"startDate", 
		L"startDate", 
		sizeof("startDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrBySubmitDate_struct, startDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x6689F575, 
		"endDate", 
		L"endDate", 
		sizeof("endDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrBySubmitDate_struct, endDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xBBE6146C, 
		"QueryFeedBackArrBySubmitDateResult", 
		L"QueryFeedBackArrBySubmitDateResult", 
		sizeof("QueryFeedBackArrBySubmitDateResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBackArrBySubmitDate_struct, QueryFeedBackArrBySubmitDateResult),
		NULL,
		&__FeedBack_map,
		3+1,
	},
	{
		0xBBE6146C,
		"__QueryFeedBackArrBySubmitDateResult_nSizeIs",
		L"__QueryFeedBackArrBySubmitDateResult_nSizeIs",
		sizeof("__QueryFeedBackArrBySubmitDateResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryFeedBackArrBySubmitDate_struct, __QueryFeedBackArrBySubmitDateResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryFeedBackArrBySubmitDate_map =
{
	0x4D63FFBC,
	"QueryFeedBackArrBySubmitDate",
	L"QueryFeedBackArrBySubmitDateResponse",
	sizeof("QueryFeedBackArrBySubmitDate")-1,
	sizeof("QueryFeedBackArrBySubmitDateResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryFeedBackArrBySubmitDate_entries,
	sizeof(__CXMServer_QueryFeedBackArrBySubmitDate_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownloadTestItemById_struct
{
	int sessionId;
	int id;
	TestItem DownloadTestItemByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownloadTestItemById_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadTestItemById_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadTestItemById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x1BDD896E, 
		"DownloadTestItemByIdResult", 
		L"DownloadTestItemByIdResult", 
		sizeof("DownloadTestItemByIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadTestItemById_struct, DownloadTestItemByIdResult),
		NULL,
		&__TestItem_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownloadTestItemById_map =
{
	0x890CBD3E,
	"DownloadTestItemById",
	L"DownloadTestItemByIdResponse",
	sizeof("DownloadTestItemById")-1,
	sizeof("DownloadTestItemByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownloadTestItemById_entries,
	sizeof(__CXMServer_DownloadTestItemById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_ValidatePassword_struct
{
	int sessionId;
	int userId;
	BSTR password;
	bool ValidatePasswordResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_ValidatePassword_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_ValidatePassword_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x1945E48C, 
		"userId", 
		L"userId", 
		sizeof("userId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_ValidatePassword_struct, userId),
		NULL,
		NULL,
		-1,
	},
	{
		0x9C439233, 
		"password", 
		L"password", 
		sizeof("password")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_ValidatePassword_struct, password),
		NULL,
		NULL,
		-1,
	},
	{
		0x6C06A35C, 
		"ValidatePasswordResult", 
		L"ValidatePasswordResult", 
		sizeof("ValidatePasswordResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_ValidatePassword_struct, ValidatePasswordResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_ValidatePassword_map =
{
	0x87E40AAC,
	"ValidatePassword",
	L"ValidatePasswordResponse",
	sizeof("ValidatePassword")-1,
	sizeof("ValidatePasswordResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_ValidatePassword_entries,
	sizeof(__CXMServer_ValidatePassword_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryTestItemByWorkLogId_struct
{
	int sessionId;
	int logId;
	TestItem *QueryTestItemByWorkLogIdResult;
	int __QueryTestItemByWorkLogIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryTestItemByWorkLogId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemByWorkLogId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x07E0F46F, 
		"logId", 
		L"logId", 
		sizeof("logId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemByWorkLogId_struct, logId),
		NULL,
		NULL,
		-1,
	},
	{
		0xBE441791, 
		"QueryTestItemByWorkLogIdResult", 
		L"QueryTestItemByWorkLogIdResult", 
		sizeof("QueryTestItemByWorkLogIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryTestItemByWorkLogId_struct, QueryTestItemByWorkLogIdResult),
		NULL,
		&__TestItem_map,
		2+1,
	},
	{
		0xBE441791,
		"__QueryTestItemByWorkLogIdResult_nSizeIs",
		L"__QueryTestItemByWorkLogIdResult_nSizeIs",
		sizeof("__QueryTestItemByWorkLogIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryTestItemByWorkLogId_struct, __QueryTestItemByWorkLogIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryTestItemByWorkLogId_map =
{
	0x5F4F6021,
	"QueryTestItemByWorkLogId",
	L"QueryTestItemByWorkLogIdResponse",
	sizeof("QueryTestItemByWorkLogId")-1,
	sizeof("QueryTestItemByWorkLogIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryTestItemByWorkLogId_entries,
	sizeof(__CXMServer_QueryTestItemByWorkLogId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryNotificationArr_struct
{
	int sessionId;
	BSTR startDate;
	BSTR endDate;
	Notification *QueryNotificationArrResult;
	int __QueryNotificationArrResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryNotificationArr_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryNotificationArr_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x9ED2666C, 
		"startDate", 
		L"startDate", 
		sizeof("startDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryNotificationArr_struct, startDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x6689F575, 
		"endDate", 
		L"endDate", 
		sizeof("endDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryNotificationArr_struct, endDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x1DB4F581, 
		"QueryNotificationArrResult", 
		L"QueryNotificationArrResult", 
		sizeof("QueryNotificationArrResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryNotificationArr_struct, QueryNotificationArrResult),
		NULL,
		&__Notification_map,
		3+1,
	},
	{
		0x1DB4F581,
		"__QueryNotificationArrResult_nSizeIs",
		L"__QueryNotificationArrResult_nSizeIs",
		sizeof("__QueryNotificationArrResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryNotificationArr_struct, __QueryNotificationArrResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryNotificationArr_map =
{
	0x5E6F7A11,
	"QueryNotificationArr",
	L"QueryNotificationArrResponse",
	sizeof("QueryNotificationArr")-1,
	sizeof("QueryNotificationArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryNotificationArr_entries,
	sizeof(__CXMServer_QueryNotificationArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_GetRelationObject_struct
{
	RelationObject GetRelationObjectResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_GetRelationObject_entries[] =
{

	{
		0xDAFAF054, 
		"GetRelationObjectResult", 
		L"GetRelationObjectResult", 
		sizeof("GetRelationObjectResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_GetRelationObject_struct, GetRelationObjectResult),
		NULL,
		&__RelationObject_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_GetRelationObject_map =
{
	0x851F75A4,
	"GetRelationObject",
	L"GetRelationObjectResponse",
	sizeof("GetRelationObject")-1,
	sizeof("GetRelationObjectResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_GetRelationObject_entries,
	sizeof(__CXMServer_GetRelationObject_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeletePartBuildByPartItemId_struct
{
	int partItemId;
	bool DeletePartBuildByPartItemIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeletePartBuildByPartItemId_entries[] =
{

	{
		0xB12F8F93, 
		"partItemId", 
		L"partItemId", 
		sizeof("partItemId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeletePartBuildByPartItemId_struct, partItemId),
		NULL,
		NULL,
		-1,
	},
	{
		0x8A0EC547, 
		"DeletePartBuildByPartItemIdResult", 
		L"DeletePartBuildByPartItemIdResult", 
		sizeof("DeletePartBuildByPartItemIdResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeletePartBuildByPartItemId_struct, DeletePartBuildByPartItemIdResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeletePartBuildByPartItemId_map =
{
	0x487C5357,
	"DeletePartBuildByPartItemId",
	L"DeletePartBuildByPartItemIdResponse",
	sizeof("DeletePartBuildByPartItemId")-1,
	sizeof("DeletePartBuildByPartItemIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeletePartBuildByPartItemId_entries,
	sizeof(__CXMServer_DeletePartBuildByPartItemId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownLoadMemberRole_struct
{
	int sessionId;
	Role DownLoadMemberRoleResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownLoadMemberRole_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadMemberRole_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xCA757DC1, 
		"DownLoadMemberRoleResult", 
		L"DownLoadMemberRoleResult", 
		sizeof("DownLoadMemberRoleResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadMemberRole_struct, DownLoadMemberRoleResult),
		NULL,
		&__Role_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownLoadMemberRole_map =
{
	0x3D731251,
	"DownLoadMemberRole",
	L"DownLoadMemberRoleResponse",
	sizeof("DownLoadMemberRole")-1,
	sizeof("DownLoadMemberRoleResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownLoadMemberRole_entries,
	sizeof(__CXMServer_DownLoadMemberRole_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteProblemGradeById_struct
{
	int sessionId;
	int id;
	bool DeleteProblemGradeByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteProblemGradeById_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteProblemGradeById_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteProblemGradeById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x6D5457EE, 
		"DeleteProblemGradeByIdResult", 
		L"DeleteProblemGradeByIdResult", 
		sizeof("DeleteProblemGradeByIdResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteProblemGradeById_struct, DeleteProblemGradeByIdResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteProblemGradeById_map =
{
	0x13712BBE,
	"DeleteProblemGradeById",
	L"DeleteProblemGradeByIdResponse",
	sizeof("DeleteProblemGradeById")-1,
	sizeof("DeleteProblemGradeByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteProblemGradeById_entries,
	sizeof(__CXMServer_DeleteProblemGradeById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryAllDownloadItem_struct
{
	int sessionId;
	DownloadItem *QueryAllDownloadItemResult;
	int __QueryAllDownloadItemResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryAllDownloadItem_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryAllDownloadItem_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x5F8C6C75, 
		"QueryAllDownloadItemResult", 
		L"QueryAllDownloadItemResult", 
		sizeof("QueryAllDownloadItemResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryAllDownloadItem_struct, QueryAllDownloadItemResult),
		NULL,
		&__DownloadItem_map,
		1+1,
	},
	{
		0x5F8C6C75,
		"__QueryAllDownloadItemResult_nSizeIs",
		L"__QueryAllDownloadItemResult_nSizeIs",
		sizeof("__QueryAllDownloadItemResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryAllDownloadItem_struct, __QueryAllDownloadItemResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryAllDownloadItem_map =
{
	0x74007E05,
	"QueryAllDownloadItem",
	L"QueryAllDownloadItemResponse",
	sizeof("QueryAllDownloadItem")-1,
	sizeof("QueryAllDownloadItemResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryAllDownloadItem_entries,
	sizeof(__CXMServer_QueryAllDownloadItem_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryAllNews_struct
{
	int sessionId;
	News *QueryAllNewsResult;
	int __QueryAllNewsResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryAllNews_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryAllNews_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x69A036AB, 
		"QueryAllNewsResult", 
		L"QueryAllNewsResult", 
		sizeof("QueryAllNewsResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryAllNews_struct, QueryAllNewsResult),
		NULL,
		&__News_map,
		1+1,
	},
	{
		0x69A036AB,
		"__QueryAllNewsResult_nSizeIs",
		L"__QueryAllNewsResult_nSizeIs",
		sizeof("__QueryAllNewsResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryAllNews_struct, __QueryAllNewsResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryAllNews_map =
{
	0x522FADBB,
	"QueryAllNews",
	L"QueryAllNewsResponse",
	sizeof("QueryAllNews")-1,
	sizeof("QueryAllNewsResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryAllNews_entries,
	sizeof(__CXMServer_QueryAllNews_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_GetNotification_struct
{
	Notification GetNotificationResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_GetNotification_entries[] =
{

	{
		0x66FE9746, 
		"GetNotificationResult", 
		L"GetNotificationResult", 
		sizeof("GetNotificationResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_GetNotification_struct, GetNotificationResult),
		NULL,
		&__Notification_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_GetNotification_map =
{
	0x20A8A116,
	"GetNotification",
	L"GetNotificationResponse",
	sizeof("GetNotification")-1,
	sizeof("GetNotificationResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_GetNotification_entries,
	sizeof(__CXMServer_GetNotification_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateDevelopLog_struct
{
	int sessionId;
	DevelopLog developLog;
	int SaveOrUpdateDevelopLogResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateDevelopLog_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateDevelopLog_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xD24FDE71, 
		"developLog", 
		L"developLog", 
		sizeof("developLog")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateDevelopLog_struct, developLog),
		NULL,
		&__DevelopLog_map,
		-1,
	},
	{
		0xC4A25D03, 
		"SaveOrUpdateDevelopLogResult", 
		L"SaveOrUpdateDevelopLogResult", 
		sizeof("SaveOrUpdateDevelopLogResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateDevelopLog_struct, SaveOrUpdateDevelopLogResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateDevelopLog_map =
{
	0x7654CA13,
	"SaveOrUpdateDevelopLog",
	L"SaveOrUpdateDevelopLogResponse",
	sizeof("SaveOrUpdateDevelopLog")-1,
	sizeof("SaveOrUpdateDevelopLogResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateDevelopLog_entries,
	sizeof(__CXMServer_SaveOrUpdateDevelopLog_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryClientArr_struct
{
	int sessionId;
	bool isProduct;
	bool isDownloadItem;
	Client *QueryClientArrResult;
	int __QueryClientArrResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryClientArr_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryClientArr_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x08CD833D, 
		"isProduct", 
		L"isProduct", 
		sizeof("isProduct")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryClientArr_struct, isProduct),
		NULL,
		NULL,
		-1,
	},
	{
		0xAD087243, 
		"isDownloadItem", 
		L"isDownloadItem", 
		sizeof("isDownloadItem")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryClientArr_struct, isDownloadItem),
		NULL,
		NULL,
		-1,
	},
	{
		0x17842039, 
		"QueryClientArrResult", 
		L"QueryClientArrResult", 
		sizeof("QueryClientArrResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryClientArr_struct, QueryClientArrResult),
		NULL,
		&__Client_map,
		3+1,
	},
	{
		0x17842039,
		"__QueryClientArrResult_nSizeIs",
		L"__QueryClientArrResult_nSizeIs",
		sizeof("__QueryClientArrResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryClientArr_struct, __QueryClientArrResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryClientArr_map =
{
	0x08B432C9,
	"QueryClientArr",
	L"QueryClientArrResponse",
	sizeof("QueryClientArr")-1,
	sizeof("QueryClientArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryClientArr_entries,
	sizeof(__CXMServer_QueryClientArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateWorkLog_struct
{
	int sessionId;
	WorkLog workLog;
	int SaveOrUpdateWorkLogResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateWorkLog_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateWorkLog_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xD389C385, 
		"workLog", 
		L"workLog", 
		sizeof("workLog")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateWorkLog_struct, workLog),
		NULL,
		&__WorkLog_map,
		-1,
	},
	{
		0xED00A5F7, 
		"SaveOrUpdateWorkLogResult", 
		L"SaveOrUpdateWorkLogResult", 
		sizeof("SaveOrUpdateWorkLogResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateWorkLog_struct, SaveOrUpdateWorkLogResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateWorkLog_map =
{
	0x2F692007,
	"SaveOrUpdateWorkLog",
	L"SaveOrUpdateWorkLogResponse",
	sizeof("SaveOrUpdateWorkLog")-1,
	sizeof("SaveOrUpdateWorkLogResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateWorkLog_entries,
	sizeof(__CXMServer_SaveOrUpdateWorkLog_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryCreateTaskUsers_struct
{
	int sessionId;
	User *QueryCreateTaskUsersResult;
	int __QueryCreateTaskUsersResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryCreateTaskUsers_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryCreateTaskUsers_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xC8D143AE, 
		"QueryCreateTaskUsersResult", 
		L"QueryCreateTaskUsersResult", 
		sizeof("QueryCreateTaskUsersResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryCreateTaskUsers_struct, QueryCreateTaskUsersResult),
		NULL,
		&__User_map,
		1+1,
	},
	{
		0xC8D143AE,
		"__QueryCreateTaskUsersResult_nSizeIs",
		L"__QueryCreateTaskUsersResult_nSizeIs",
		sizeof("__QueryCreateTaskUsersResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryCreateTaskUsers_struct, __QueryCreateTaskUsersResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryCreateTaskUsers_map =
{
	0x41D8077E,
	"QueryCreateTaskUsers",
	L"QueryCreateTaskUsersResponse",
	sizeof("QueryCreateTaskUsers")-1,
	sizeof("QueryCreateTaskUsersResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryCreateTaskUsers_entries,
	sizeof(__CXMServer_QueryCreateTaskUsers_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteClient_ProductionByClient_struct
{
	int clientId;
	bool DeleteClient_ProductionByClientResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteClient_ProductionByClient_entries[] =
{

	{
		0xD356924C, 
		"clientId", 
		L"clientId", 
		sizeof("clientId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteClient_ProductionByClient_struct, clientId),
		NULL,
		NULL,
		-1,
	},
	{
		0x623DCE51, 
		"DeleteClient_ProductionByClientResult", 
		L"DeleteClient_ProductionByClientResult", 
		sizeof("DeleteClient_ProductionByClientResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteClient_ProductionByClient_struct, DeleteClient_ProductionByClientResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteClient_ProductionByClient_map =
{
	0xE891C6E1,
	"DeleteClient_ProductionByClient",
	L"DeleteClient_ProductionByClientResponse",
	sizeof("DeleteClient_ProductionByClient")-1,
	sizeof("DeleteClient_ProductionByClientResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteClient_ProductionByClient_entries,
	sizeof(__CXMServer_DeleteClient_ProductionByClient_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateReply_struct
{
	int sessionId;
	Reply reply;
	bool isFile;
	int SaveOrUpdateReplyResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateReply_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateReply_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x0848368C, 
		"reply", 
		L"reply", 
		sizeof("reply")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateReply_struct, reply),
		NULL,
		&__Reply_map,
		-1,
	},
	{
		0xFD36DB7C, 
		"isFile", 
		L"isFile", 
		sizeof("isFile")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateReply_struct, isFile),
		NULL,
		NULL,
		-1,
	},
	{
		0x944CC17E, 
		"SaveOrUpdateReplyResult", 
		L"SaveOrUpdateReplyResult", 
		sizeof("SaveOrUpdateReplyResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateReply_struct, SaveOrUpdateReplyResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateReply_map =
{
	0xDA2A394E,
	"SaveOrUpdateReply",
	L"SaveOrUpdateReplyResponse",
	sizeof("SaveOrUpdateReply")-1,
	sizeof("SaveOrUpdateReplyResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateReply_entries,
	sizeof(__CXMServer_SaveOrUpdateReply_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteProduction_struct
{
	int sessionId;
	int productId;
	bool DeleteProductionResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteProduction_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteProduction_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x7E03E6EE, 
		"productId", 
		L"productId", 
		sizeof("productId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteProduction_struct, productId),
		NULL,
		NULL,
		-1,
	},
	{
		0x399AA379, 
		"DeleteProductionResult", 
		L"DeleteProductionResult", 
		sizeof("DeleteProductionResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteProduction_struct, DeleteProductionResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteProduction_map =
{
	0x0A788609,
	"DeleteProduction",
	L"DeleteProductionResponse",
	sizeof("DeleteProduction")-1,
	sizeof("DeleteProductionResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteProduction_entries,
	sizeof(__CXMServer_DeleteProduction_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_GetProduction_struct
{
	Production GetProductionResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_GetProduction_entries[] =
{

	{
		0x9710AD66, 
		"GetProductionResult", 
		L"GetProductionResult", 
		sizeof("GetProductionResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_GetProduction_struct, GetProductionResult),
		NULL,
		&__Production_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_GetProduction_map =
{
	0x9D98BF36,
	"GetProduction",
	L"GetProductionResponse",
	sizeof("GetProduction")-1,
	sizeof("GetProductionResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_GetProduction_entries,
	sizeof(__CXMServer_GetProduction_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownLoadTechnoiogyRole_struct
{
	int sessionId;
	Role DownLoadTechnoiogyRoleResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownLoadTechnoiogyRole_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadTechnoiogyRole_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xF1228C82, 
		"DownLoadTechnoiogyRoleResult", 
		L"DownLoadTechnoiogyRoleResult", 
		sizeof("DownLoadTechnoiogyRoleResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownLoadTechnoiogyRole_struct, DownLoadTechnoiogyRoleResult),
		NULL,
		&__Role_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownLoadTechnoiogyRole_map =
{
	0xC39ED552,
	"DownLoadTechnoiogyRole",
	L"DownLoadTechnoiogyRoleResponse",
	sizeof("DownLoadTechnoiogyRole")-1,
	sizeof("DownLoadTechnoiogyRoleResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownLoadTechnoiogyRole_entries,
	sizeof(__CXMServer_DownLoadTechnoiogyRole_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryFeedBackArrByVersionRevision_struct
{
	int sessionId;
	int versionRevisionId;
	FeedBack *QueryFeedBackArrByVersionRevisionResult;
	int __QueryFeedBackArrByVersionRevisionResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryFeedBackArrByVersionRevision_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrByVersionRevision_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xB7C74C42, 
		"versionRevisionId", 
		L"versionRevisionId", 
		sizeof("versionRevisionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBackArrByVersionRevision_struct, versionRevisionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xAEAC1AEF, 
		"QueryFeedBackArrByVersionRevisionResult", 
		L"QueryFeedBackArrByVersionRevisionResult", 
		sizeof("QueryFeedBackArrByVersionRevisionResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBackArrByVersionRevision_struct, QueryFeedBackArrByVersionRevisionResult),
		NULL,
		&__FeedBack_map,
		2+1,
	},
	{
		0xAEAC1AEF,
		"__QueryFeedBackArrByVersionRevisionResult_nSizeIs",
		L"__QueryFeedBackArrByVersionRevisionResult_nSizeIs",
		sizeof("__QueryFeedBackArrByVersionRevisionResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryFeedBackArrByVersionRevision_struct, __QueryFeedBackArrByVersionRevisionResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryFeedBackArrByVersionRevision_map =
{
	0x09C5B2FF,
	"QueryFeedBackArrByVersionRevision",
	L"QueryFeedBackArrByVersionRevisionResponse",
	sizeof("QueryFeedBackArrByVersionRevision")-1,
	sizeof("QueryFeedBackArrByVersionRevisionResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryFeedBackArrByVersionRevision_entries,
	sizeof(__CXMServer_QueryFeedBackArrByVersionRevision_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateAttendance_struct
{
	int sessionId;
	Attendance attendance;
	int SaveOrUpdateAttendanceResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateAttendance_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateAttendance_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xC4D2B977, 
		"attendance", 
		L"attendance", 
		sizeof("attendance")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateAttendance_struct, attendance),
		NULL,
		&__Attendance_map,
		-1,
	},
	{
		0xF07AE489, 
		"SaveOrUpdateAttendanceResult", 
		L"SaveOrUpdateAttendanceResult", 
		sizeof("SaveOrUpdateAttendanceResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateAttendance_struct, SaveOrUpdateAttendanceResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateAttendance_map =
{
	0xFA6D4B19,
	"SaveOrUpdateAttendance",
	L"SaveOrUpdateAttendanceResponse",
	sizeof("SaveOrUpdateAttendance")-1,
	sizeof("SaveOrUpdateAttendanceResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateAttendance_entries,
	sizeof(__CXMServer_SaveOrUpdateAttendance_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteClientDownloadByClientId_struct
{
	int sessionId;
	int id;
	bool DeleteClientDownloadByClientIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteClientDownloadByClientId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteClientDownloadByClientId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteClientDownloadByClientId_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x393AF1B0, 
		"DeleteClientDownloadByClientIdResult", 
		L"DeleteClientDownloadByClientIdResult", 
		sizeof("DeleteClientDownloadByClientIdResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteClientDownloadByClientId_struct, DeleteClientDownloadByClientIdResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteClientDownloadByClientId_map =
{
	0x73653E00,
	"DeleteClientDownloadByClientId",
	L"DeleteClientDownloadByClientIdResponse",
	sizeof("DeleteClientDownloadByClientId")-1,
	sizeof("DeleteClientDownloadByClientIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteClientDownloadByClientId_entries,
	sizeof(__CXMServer_DeleteClientDownloadByClientId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdateRole_struct
{
	int sessionId;
	Role role;
	int SaveOrUpdateRoleResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdateRole_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateRole_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x004069B2, 
		"role", 
		L"role", 
		sizeof("role")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateRole_struct, role),
		NULL,
		&__Role_map,
		-1,
	},
	{
		0xC1378AC4, 
		"SaveOrUpdateRoleResult", 
		L"SaveOrUpdateRoleResult", 
		sizeof("SaveOrUpdateRoleResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdateRole_struct, SaveOrUpdateRoleResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdateRole_map =
{
	0xECEC6C14,
	"SaveOrUpdateRole",
	L"SaveOrUpdateRoleResponse",
	sizeof("SaveOrUpdateRole")-1,
	sizeof("SaveOrUpdateRoleResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdateRole_entries,
	sizeof(__CXMServer_SaveOrUpdateRole_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryAllFeedBackArr_struct
{
	int sessionId;
	FeedBack *QueryAllFeedBackArrResult;
	int __QueryAllFeedBackArrResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryAllFeedBackArr_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryAllFeedBackArr_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xEAA59A78, 
		"QueryAllFeedBackArrResult", 
		L"QueryAllFeedBackArrResult", 
		sizeof("QueryAllFeedBackArrResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryAllFeedBackArr_struct, QueryAllFeedBackArrResult),
		NULL,
		&__FeedBack_map,
		1+1,
	},
	{
		0xEAA59A78,
		"__QueryAllFeedBackArrResult_nSizeIs",
		L"__QueryAllFeedBackArrResult_nSizeIs",
		sizeof("__QueryAllFeedBackArrResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryAllFeedBackArr_struct, __QueryAllFeedBackArrResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryAllFeedBackArr_map =
{
	0x2A1D38C8,
	"QueryAllFeedBackArr",
	L"QueryAllFeedBackArrResponse",
	sizeof("QueryAllFeedBackArr")-1,
	sizeof("QueryAllFeedBackArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryAllFeedBackArr_entries,
	sizeof(__CXMServer_QueryAllFeedBackArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteFeedBackGroup_struct
{
	int sessionId;
	int groupId;
	bool DeleteFeedBackGroupResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteFeedBackGroup_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteFeedBackGroup_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x0AB537DA, 
		"groupId", 
		L"groupId", 
		sizeof("groupId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteFeedBackGroup_struct, groupId),
		NULL,
		NULL,
		-1,
	},
	{
		0xBE82E884, 
		"DeleteFeedBackGroupResult", 
		L"DeleteFeedBackGroupResult", 
		sizeof("DeleteFeedBackGroupResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteFeedBackGroup_struct, DeleteFeedBackGroupResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteFeedBackGroup_map =
{
	0x6A8639D4,
	"DeleteFeedBackGroup",
	L"DeleteFeedBackGroupResponse",
	sizeof("DeleteFeedBackGroup")-1,
	sizeof("DeleteFeedBackGroupResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteFeedBackGroup_entries,
	sizeof(__CXMServer_DeleteFeedBackGroup_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SaveOrUpdatePartBuild_struct
{
	int sessionId;
	PartBuild pb;
	int SaveOrUpdatePartBuildResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SaveOrUpdatePartBuild_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdatePartBuild_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000ED2, 
		"pb", 
		L"pb", 
		sizeof("pb")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdatePartBuild_struct, pb),
		NULL,
		&__PartBuild_map,
		-1,
	},
	{
		0x630FDBD9, 
		"SaveOrUpdatePartBuildResult", 
		L"SaveOrUpdatePartBuildResult", 
		sizeof("SaveOrUpdatePartBuildResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SaveOrUpdatePartBuild_struct, SaveOrUpdatePartBuildResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SaveOrUpdatePartBuild_map =
{
	0x661D5669,
	"SaveOrUpdatePartBuild",
	L"SaveOrUpdatePartBuildResponse",
	sizeof("SaveOrUpdatePartBuild")-1,
	sizeof("SaveOrUpdatePartBuildResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SaveOrUpdatePartBuild_entries,
	sizeof(__CXMServer_SaveOrUpdatePartBuild_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteDevelopLopById_struct
{
	int sessionId;
	int id;
	bool DeleteDevelopLopByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteDevelopLopById_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteDevelopLopById_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteDevelopLopById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xA2E8B794, 
		"DeleteDevelopLopByIdResult", 
		L"DeleteDevelopLopByIdResult", 
		sizeof("DeleteDevelopLopByIdResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteDevelopLopById_struct, DeleteDevelopLopByIdResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteDevelopLopById_map =
{
	0xFF9C0CE4,
	"DeleteDevelopLopById",
	L"DeleteDevelopLopByIdResponse",
	sizeof("DeleteDevelopLopById")-1,
	sizeof("DeleteDevelopLopByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteDevelopLopById_entries,
	sizeof(__CXMServer_DeleteDevelopLopById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownloadWorkLogById_struct
{
	int sessionId;
	int id;
	WorkLog DownloadWorkLogByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownloadWorkLogById_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadWorkLogById_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadWorkLogById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xEE05C204, 
		"DownloadWorkLogByIdResult", 
		L"DownloadWorkLogByIdResult", 
		sizeof("DownloadWorkLogByIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadWorkLogById_struct, DownloadWorkLogByIdResult),
		NULL,
		&__WorkLog_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownloadWorkLogById_map =
{
	0x86257354,
	"DownloadWorkLogById",
	L"DownloadWorkLogByIdResponse",
	sizeof("DownloadWorkLogById")-1,
	sizeof("DownloadWorkLogByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownloadWorkLogById_entries,
	sizeof(__CXMServer_DownloadWorkLogById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryAttendanceArrByUserId_struct
{
	int sessionId;
	int userId;
	BSTR startDate;
	BSTR endDate;
	Attendance *QueryAttendanceArrByUserIdResult;
	int __QueryAttendanceArrByUserIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryAttendanceArrByUserId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryAttendanceArrByUserId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x1945E48C, 
		"userId", 
		L"userId", 
		sizeof("userId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryAttendanceArrByUserId_struct, userId),
		NULL,
		NULL,
		-1,
	},
	{
		0x9ED2666C, 
		"startDate", 
		L"startDate", 
		sizeof("startDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryAttendanceArrByUserId_struct, startDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x6689F575, 
		"endDate", 
		L"endDate", 
		sizeof("endDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryAttendanceArrByUserId_struct, endDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xE6FF63D8, 
		"QueryAttendanceArrByUserIdResult", 
		L"QueryAttendanceArrByUserIdResult", 
		sizeof("QueryAttendanceArrByUserIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryAttendanceArrByUserId_struct, QueryAttendanceArrByUserIdResult),
		NULL,
		&__Attendance_map,
		4+1,
	},
	{
		0xE6FF63D8,
		"__QueryAttendanceArrByUserIdResult_nSizeIs",
		L"__QueryAttendanceArrByUserIdResult_nSizeIs",
		sizeof("__QueryAttendanceArrByUserIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryAttendanceArrByUserId_struct, __QueryAttendanceArrByUserIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryAttendanceArrByUserId_map =
{
	0xA40EDA28,
	"QueryAttendanceArrByUserId",
	L"QueryAttendanceArrByUserIdResponse",
	sizeof("QueryAttendanceArrByUserId")-1,
	sizeof("QueryAttendanceArrByUserIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryAttendanceArrByUserId_entries,
	sizeof(__CXMServer_QueryAttendanceArrByUserId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteJobById_struct
{
	int sessionId;
	int id;
	bool DeleteJobByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteJobById_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteJobById_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteJobById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x938E0355, 
		"DeleteJobByIdResult", 
		L"DeleteJobByIdResult", 
		sizeof("DeleteJobByIdResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteJobById_struct, DeleteJobByIdResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteJobById_map =
{
	0xAEC34CE5,
	"DeleteJobById",
	L"DeleteJobByIdResponse",
	sizeof("DeleteJobById")-1,
	sizeof("DeleteJobByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteJobById_entries,
	sizeof(__CXMServer_DeleteJobById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_UpdateTasksSerialNo_struct
{
	int sessionId;
	int *idArr;
	int __idArr_nSizeIs;
	int *serialArr;
	int __serialArr_nSizeIs;
	bool UpdateTasksSerialNoResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_UpdateTasksSerialNo_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateTasksSerialNo_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x07A40672, 
		"idArr", 
		L"idArr", 
		sizeof("idArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_UpdateTasksSerialNo_struct, idArr),
		NULL,
		NULL,
		1+1,
	},
	{
		0x07A40672,
		"__idArr_nSizeIs",
		L"__idArr_nSizeIs",
		sizeof("__idArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_UpdateTasksSerialNo_struct, __idArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xCD541DA5, 
		"serialArr", 
		L"serialArr", 
		sizeof("serialArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_UpdateTasksSerialNo_struct, serialArr),
		NULL,
		NULL,
		3+1,
	},
	{
		0xCD541DA5,
		"__serialArr_nSizeIs",
		L"__serialArr_nSizeIs",
		sizeof("__serialArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_UpdateTasksSerialNo_struct, __serialArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1675C825, 
		"UpdateTasksSerialNoResult", 
		L"UpdateTasksSerialNoResult", 
		sizeof("UpdateTasksSerialNoResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateTasksSerialNo_struct, UpdateTasksSerialNoResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_UpdateTasksSerialNo_map =
{
	0x8AAF85B5,
	"UpdateTasksSerialNo",
	L"UpdateTasksSerialNoResponse",
	sizeof("UpdateTasksSerialNo")-1,
	sizeof("UpdateTasksSerialNoResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_UpdateTasksSerialNo_entries,
	sizeof(__CXMServer_UpdateTasksSerialNo_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryAllUser_struct
{
	int sessionId;
	User *QueryAllUserResult;
	int __QueryAllUserResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryAllUser_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryAllUser_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xFB1D280D, 
		"QueryAllUserResult", 
		L"QueryAllUserResult", 
		sizeof("QueryAllUserResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryAllUser_struct, QueryAllUserResult),
		NULL,
		&__User_map,
		1+1,
	},
	{
		0xFB1D280D,
		"__QueryAllUserResult_nSizeIs",
		L"__QueryAllUserResult_nSizeIs",
		sizeof("__QueryAllUserResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryAllUser_struct, __QueryAllUserResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryAllUser_map =
{
	0x36AE7F9D,
	"QueryAllUser",
	L"QueryAllUserResponse",
	sizeof("QueryAllUser")-1,
	sizeof("QueryAllUserResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryAllUser_entries,
	sizeof(__CXMServer_QueryAllUser_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_canExecuteFuncOper_struct
{
	int sessionId;
	int funcOperId;
	bool canExecuteFuncOperResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_canExecuteFuncOper_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_canExecuteFuncOper_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xE290030F, 
		"funcOperId", 
		L"funcOperId", 
		sizeof("funcOperId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_canExecuteFuncOper_struct, funcOperId),
		NULL,
		NULL,
		-1,
	},
	{
		0x2A4413C6, 
		"canExecuteFuncOperResult", 
		L"canExecuteFuncOperResult", 
		sizeof("canExecuteFuncOperResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_canExecuteFuncOper_struct, canExecuteFuncOperResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_canExecuteFuncOper_map =
{
	0xCB3F3D96,
	"canExecuteFuncOper",
	L"canExecuteFuncOperResponse",
	sizeof("canExecuteFuncOper")-1,
	sizeof("canExecuteFuncOperResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_canExecuteFuncOper_entries,
	sizeof(__CXMServer_canExecuteFuncOper_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteFeedBack_struct
{
	int sessionId;
	int id;
	bool DeleteFeedBackResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteFeedBack_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteFeedBack_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteFeedBack_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xC0048917, 
		"DeleteFeedBackResult", 
		L"DeleteFeedBackResult", 
		sizeof("DeleteFeedBackResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteFeedBack_struct, DeleteFeedBackResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteFeedBack_map =
{
	0xD2F24B27,
	"DeleteFeedBack",
	L"DeleteFeedBackResponse",
	sizeof("DeleteFeedBack")-1,
	sizeof("DeleteFeedBackResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteFeedBack_entries,
	sizeof(__CXMServer_DeleteFeedBack_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeletePartBuildById_struct
{
	int sessionId;
	int id;
	bool DeletePartBuildByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeletePartBuildById_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeletePartBuildById_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeletePartBuildById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x849D9A21, 
		"DeletePartBuildByIdResult", 
		L"DeletePartBuildByIdResult", 
		sizeof("DeletePartBuildByIdResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeletePartBuildById_struct, DeletePartBuildByIdResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeletePartBuildById_map =
{
	0x2213C6B1,
	"DeletePartBuildById",
	L"DeletePartBuildByIdResponse",
	sizeof("DeletePartBuildById")-1,
	sizeof("DeletePartBuildByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeletePartBuildById_entries,
	sizeof(__CXMServer_DeletePartBuildById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryTestItemByVersionId_struct
{
	int sessionId;
	int versionId;
	TestItem *QueryTestItemByVersionIdResult;
	int __QueryTestItemByVersionIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryTestItemByVersionId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemByVersionId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x42B02A33, 
		"versionId", 
		L"versionId", 
		sizeof("versionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTestItemByVersionId_struct, versionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x9CFCF3F2, 
		"QueryTestItemByVersionIdResult", 
		L"QueryTestItemByVersionIdResult", 
		sizeof("QueryTestItemByVersionIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryTestItemByVersionId_struct, QueryTestItemByVersionIdResult),
		NULL,
		&__TestItem_map,
		2+1,
	},
	{
		0x9CFCF3F2,
		"__QueryTestItemByVersionIdResult_nSizeIs",
		L"__QueryTestItemByVersionIdResult_nSizeIs",
		sizeof("__QueryTestItemByVersionIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryTestItemByVersionId_struct, __QueryTestItemByVersionIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryTestItemByVersionId_map =
{
	0xCFB0D8C2,
	"QueryTestItemByVersionId",
	L"QueryTestItemByVersionIdResponse",
	sizeof("QueryTestItemByVersionId")-1,
	sizeof("QueryTestItemByVersionIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryTestItemByVersionId_entries,
	sizeof(__CXMServer_QueryTestItemByVersionId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryVersionUpdateInfoArr_struct
{
	BSTR production;
	BSTR version;
	VersionInfo *QueryVersionUpdateInfoArrResult;
	int __QueryVersionUpdateInfoArrResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryVersionUpdateInfoArr_entries[] =
{

	{
		0x3E814EA7, 
		"production", 
		L"production", 
		sizeof("production")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryVersionUpdateInfoArr_struct, production),
		NULL,
		NULL,
		-1,
	},
	{
		0x6F40CEE6, 
		"version", 
		L"version", 
		sizeof("version")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryVersionUpdateInfoArr_struct, version),
		NULL,
		NULL,
		-1,
	},
	{
		0x2ADFD34F, 
		"QueryVersionUpdateInfoArrResult", 
		L"QueryVersionUpdateInfoArrResult", 
		sizeof("QueryVersionUpdateInfoArrResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryVersionUpdateInfoArr_struct, QueryVersionUpdateInfoArrResult),
		NULL,
		&__VersionInfo_map,
		2+1,
	},
	{
		0x2ADFD34F,
		"__QueryVersionUpdateInfoArrResult_nSizeIs",
		L"__QueryVersionUpdateInfoArrResult_nSizeIs",
		sizeof("__QueryVersionUpdateInfoArrResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryVersionUpdateInfoArr_struct, __QueryVersionUpdateInfoArrResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryVersionUpdateInfoArr_map =
{
	0x61C9035F,
	"QueryVersionUpdateInfoArr",
	L"QueryVersionUpdateInfoArrResponse",
	sizeof("QueryVersionUpdateInfoArr")-1,
	sizeof("QueryVersionUpdateInfoArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryVersionUpdateInfoArr_entries,
	sizeof(__CXMServer_QueryVersionUpdateInfoArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryCategoryArr_struct
{
	int sessionId;
	Category *QueryCategoryArrResult;
	int __QueryCategoryArrResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryCategoryArr_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryCategoryArr_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xE08AF238, 
		"QueryCategoryArrResult", 
		L"QueryCategoryArrResult", 
		sizeof("QueryCategoryArrResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryCategoryArr_struct, QueryCategoryArrResult),
		NULL,
		&__Category_map,
		1+1,
	},
	{
		0xE08AF238,
		"__QueryCategoryArrResult_nSizeIs",
		L"__QueryCategoryArrResult_nSizeIs",
		sizeof("__QueryCategoryArrResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryCategoryArr_struct, __QueryCategoryArrResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryCategoryArr_map =
{
	0x2EB78088,
	"QueryCategoryArr",
	L"QueryCategoryArrResponse",
	sizeof("QueryCategoryArr")-1,
	sizeof("QueryCategoryArrResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryCategoryArr_entries,
	sizeof(__CXMServer_QueryCategoryArr_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryFeedBacks_struct
{
	int sessionId;
	int gradeId;
	BSTR problemType;
	BSTR processState;
	int *productIdArr;
	int __productIdArr_nSizeIs;
	int pageNum;
	int pageSize;
	FeedBack *QueryFeedBacksResult;
	int __QueryFeedBacksResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryFeedBacks_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBacks_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x09AE5F90, 
		"gradeId", 
		L"gradeId", 
		sizeof("gradeId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBacks_struct, gradeId),
		NULL,
		NULL,
		-1,
	},
	{
		0x857BA3F3, 
		"problemType", 
		L"problemType", 
		sizeof("problemType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBacks_struct, problemType),
		NULL,
		NULL,
		-1,
	},
	{
		0x7738F3A0, 
		"processState", 
		L"processState", 
		sizeof("processState")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBacks_struct, processState),
		NULL,
		NULL,
		-1,
	},
	{
		0xE1C5CBD3, 
		"productIdArr", 
		L"productIdArr", 
		sizeof("productIdArr")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBacks_struct, productIdArr),
		NULL,
		NULL,
		4+1,
	},
	{
		0xE1C5CBD3,
		"__productIdArr_nSizeIs",
		L"__productIdArr_nSizeIs",
		sizeof("__productIdArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryFeedBacks_struct, __productIdArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x973F92ED, 
		"pageNum", 
		L"pageNum", 
		sizeof("pageNum")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBacks_struct, pageNum),
		NULL,
		NULL,
		-1,
	},
	{
		0x7F347D78, 
		"pageSize", 
		L"pageSize", 
		sizeof("pageSize")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryFeedBacks_struct, pageSize),
		NULL,
		NULL,
		-1,
	},
	{
		0x0B25D2CD, 
		"QueryFeedBacksResult", 
		L"QueryFeedBacksResult", 
		sizeof("QueryFeedBacksResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryFeedBacks_struct, QueryFeedBacksResult),
		NULL,
		&__FeedBack_map,
		8+1,
	},
	{
		0x0B25D2CD,
		"__QueryFeedBacksResult_nSizeIs",
		L"__QueryFeedBacksResult_nSizeIs",
		sizeof("__QueryFeedBacksResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryFeedBacks_struct, __QueryFeedBacksResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryFeedBacks_map =
{
	0x6B8CDA5D,
	"QueryFeedBacks",
	L"QueryFeedBacksResponse",
	sizeof("QueryFeedBacks")-1,
	sizeof("QueryFeedBacksResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryFeedBacks_entries,
	sizeof(__CXMServer_QueryFeedBacks_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_InserClientDownloadItem_struct
{
	int sessionId;
	Client_DownloadItem cd;
	bool InserClientDownloadItemResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_InserClientDownloadItem_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_InserClientDownloadItem_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000D27, 
		"cd", 
		L"cd", 
		sizeof("cd")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_InserClientDownloadItem_struct, cd),
		NULL,
		&__Client_DownloadItem_map,
		-1,
	},
	{
		0x24496426, 
		"InserClientDownloadItemResult", 
		L"InserClientDownloadItemResult", 
		sizeof("InserClientDownloadItemResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_InserClientDownloadItem_struct, InserClientDownloadItemResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_InserClientDownloadItem_map =
{
	0x5BDA25F6,
	"InserClientDownloadItem",
	L"InserClientDownloadItemResponse",
	sizeof("InserClientDownloadItem")-1,
	sizeof("InserClientDownloadItemResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_InserClientDownloadItem_entries,
	sizeof(__CXMServer_InserClientDownloadItem_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteVersionRevision_struct
{
	int sessionId;
	int id;
	bool DeleteVersionRevisionResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteVersionRevision_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteVersionRevision_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteVersionRevision_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x92690867, 
		"DeleteVersionRevisionResult", 
		L"DeleteVersionRevisionResult", 
		sizeof("DeleteVersionRevisionResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteVersionRevision_struct, DeleteVersionRevisionResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteVersionRevision_map =
{
	0xD073DE77,
	"DeleteVersionRevision",
	L"DeleteVersionRevisionResponse",
	sizeof("DeleteVersionRevision")-1,
	sizeof("DeleteVersionRevisionResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteVersionRevision_entries,
	sizeof(__CXMServer_DeleteVersionRevision_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_UpdateWorklog_testItem_struct
{
	int sessionId;
	WorLog_testItem *worklogTestItemArr;
	int __worklogTestItemArr_nSizeIs;
	WorkLog wl;
	bool UpdateWorklog_testItemResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_UpdateWorklog_testItem_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateWorklog_testItem_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0xE824DF19, 
		"worklogTestItemArr", 
		L"worklogTestItemArr", 
		sizeof("worklogTestItemArr")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_UpdateWorklog_testItem_struct, worklogTestItemArr),
		NULL,
		&__WorLog_testItem_map,
		1+1,
	},
	{
		0xE824DF19,
		"__worklogTestItemArr_nSizeIs",
		L"__worklogTestItemArr_nSizeIs",
		sizeof("__worklogTestItemArr_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_UpdateWorklog_testItem_struct, __worklogTestItemArr_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x00000FC3, 
		"wl", 
		L"wl", 
		sizeof("wl")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateWorklog_testItem_struct, wl),
		NULL,
		&__WorkLog_map,
		-1,
	},
	{
		0x189EC055, 
		"UpdateWorklog_testItemResult", 
		L"UpdateWorklog_testItemResult", 
		sizeof("UpdateWorklog_testItemResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateWorklog_testItem_struct, UpdateWorklog_testItemResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_UpdateWorklog_testItem_map =
{
	0xBAF749E5,
	"UpdateWorklog_testItem",
	L"UpdateWorklog_testItemResponse",
	sizeof("UpdateWorklog_testItem")-1,
	sizeof("UpdateWorklog_testItemResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_UpdateWorklog_testItem_entries,
	sizeof(__CXMServer_UpdateWorklog_testItem_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryTaskDispatchArrByReceiverId_struct
{
	int sessionId;
	int userId;
	bool finished;
	bool unFinished;
	BSTR startDate;
	BSTR endDate;
	TaskDispatch *QueryTaskDispatchArrByReceiverIdResult;
	int __QueryTaskDispatchArrByReceiverIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryTaskDispatchArrByReceiverId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTaskDispatchArrByReceiverId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x1945E48C, 
		"userId", 
		L"userId", 
		sizeof("userId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTaskDispatchArrByReceiverId_struct, userId),
		NULL,
		NULL,
		-1,
	},
	{
		0xBD22164A, 
		"finished", 
		L"finished", 
		sizeof("finished")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTaskDispatchArrByReceiverId_struct, finished),
		NULL,
		NULL,
		-1,
	},
	{
		0x1B345CAD, 
		"unFinished", 
		L"unFinished", 
		sizeof("unFinished")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTaskDispatchArrByReceiverId_struct, unFinished),
		NULL,
		NULL,
		-1,
	},
	{
		0x9ED2666C, 
		"startDate", 
		L"startDate", 
		sizeof("startDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTaskDispatchArrByReceiverId_struct, startDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x6689F575, 
		"endDate", 
		L"endDate", 
		sizeof("endDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryTaskDispatchArrByReceiverId_struct, endDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x67A20B7A, 
		"QueryTaskDispatchArrByReceiverIdResult", 
		L"QueryTaskDispatchArrByReceiverIdResult", 
		sizeof("QueryTaskDispatchArrByReceiverIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryTaskDispatchArrByReceiverId_struct, QueryTaskDispatchArrByReceiverIdResult),
		NULL,
		&__TaskDispatch_map,
		6+1,
	},
	{
		0x67A20B7A,
		"__QueryTaskDispatchArrByReceiverIdResult_nSizeIs",
		L"__QueryTaskDispatchArrByReceiverIdResult_nSizeIs",
		sizeof("__QueryTaskDispatchArrByReceiverIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryTaskDispatchArrByReceiverId_struct, __QueryTaskDispatchArrByReceiverIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryTaskDispatchArrByReceiverId_map =
{
	0xD7F9F24A,
	"QueryTaskDispatchArrByReceiverId",
	L"QueryTaskDispatchArrByReceiverIdResponse",
	sizeof("QueryTaskDispatchArrByReceiverId")-1,
	sizeof("QueryTaskDispatchArrByReceiverIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryTaskDispatchArrByReceiverId_entries,
	sizeof(__CXMServer_QueryTaskDispatchArrByReceiverId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DeleteNewsById_struct
{
	int sessionId;
	int id;
	bool DeleteNewsByIdResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DeleteNewsById_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteNewsById_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteNewsById_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xADB4D5B7, 
		"DeleteNewsByIdResult", 
		L"DeleteNewsByIdResult", 
		sizeof("DeleteNewsByIdResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DeleteNewsById_struct, DeleteNewsByIdResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DeleteNewsById_map =
{
	0xEDE83FC7,
	"DeleteNewsById",
	L"DeleteNewsByIdResponse",
	sizeof("DeleteNewsById")-1,
	sizeof("DeleteNewsByIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DeleteNewsById_entries,
	sizeof(__CXMServer_DeleteNewsById_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownloadUser_struct
{
	int sessionId;
	int id;
	User DownloadUserResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownloadUser_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadUser_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadUser_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0xBCF72DF6, 
		"DownloadUserResult", 
		L"DownloadUserResult", 
		sizeof("DownloadUserResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadUser_struct, DownloadUserResult),
		NULL,
		&__User_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownloadUser_map =
{
	0xD721A3C6,
	"DownloadUser",
	L"DownloadUserResponse",
	sizeof("DownloadUser")-1,
	sizeof("DownloadUserResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownloadUser_entries,
	sizeof(__CXMServer_DownloadUser_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_LoginUser_struct
{
	BSTR userName;
	BSTR password;
	LoginClass LoginUserResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_LoginUser_entries[] =
{

	{
		0x8253F720, 
		"userName", 
		L"userName", 
		sizeof("userName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_LoginUser_struct, userName),
		NULL,
		NULL,
		-1,
	},
	{
		0x9C439233, 
		"password", 
		L"password", 
		sizeof("password")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_LoginUser_struct, password),
		NULL,
		NULL,
		-1,
	},
	{
		0x6A6E4FF7, 
		"LoginUserResult", 
		L"LoginUserResult", 
		sizeof("LoginUserResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_LoginUser_struct, LoginUserResult),
		NULL,
		&__LoginClass_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_LoginUser_map =
{
	0xBEE94A07,
	"LoginUser",
	L"LoginUserResponse",
	sizeof("LoginUser")-1,
	sizeof("LoginUserResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_LoginUser_entries,
	sizeof(__CXMServer_LoginUser_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryAllPartBuild_struct
{
	PartBuild *QueryAllPartBuildResult;
	int __QueryAllPartBuildResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryAllPartBuild_entries[] =
{

	{
		0x50AE8395, 
		"QueryAllPartBuildResult", 
		L"QueryAllPartBuildResult", 
		sizeof("QueryAllPartBuildResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryAllPartBuild_struct, QueryAllPartBuildResult),
		NULL,
		&__PartBuild_map,
		0+1,
	},
	{
		0x50AE8395,
		"__QueryAllPartBuildResult_nSizeIs",
		L"__QueryAllPartBuildResult_nSizeIs",
		sizeof("__QueryAllPartBuildResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryAllPartBuild_struct, __QueryAllPartBuildResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryAllPartBuild_map =
{
	0x3604DD25,
	"QueryAllPartBuild",
	L"QueryAllPartBuildResponse",
	sizeof("QueryAllPartBuild")-1,
	sizeof("QueryAllPartBuildResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryAllPartBuild_entries,
	sizeof(__CXMServer_QueryAllPartBuild_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryVersionTestItemArrByVersionId_struct
{
	int sessionId;
	int versionId;
	Version_testItem *QueryVersionTestItemArrByVersionIdResult;
	int __QueryVersionTestItemArrByVersionIdResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryVersionTestItemArrByVersionId_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryVersionTestItemArrByVersionId_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x42B02A33, 
		"versionId", 
		L"versionId", 
		sizeof("versionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryVersionTestItemArrByVersionId_struct, versionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x9A51931D, 
		"QueryVersionTestItemArrByVersionIdResult", 
		L"QueryVersionTestItemArrByVersionIdResult", 
		sizeof("QueryVersionTestItemArrByVersionIdResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryVersionTestItemArrByVersionId_struct, QueryVersionTestItemArrByVersionIdResult),
		NULL,
		&__Version_testItem_map,
		2+1,
	},
	{
		0x9A51931D,
		"__QueryVersionTestItemArrByVersionIdResult_nSizeIs",
		L"__QueryVersionTestItemArrByVersionIdResult_nSizeIs",
		sizeof("__QueryVersionTestItemArrByVersionIdResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryVersionTestItemArrByVersionId_struct, __QueryVersionTestItemArrByVersionIdResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryVersionTestItemArrByVersionId_map =
{
	0x74A9EEAD,
	"QueryVersionTestItemArrByVersionId",
	L"QueryVersionTestItemArrByVersionIdResponse",
	sizeof("QueryVersionTestItemArrByVersionId")-1,
	sizeof("QueryVersionTestItemArrByVersionIdResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryVersionTestItemArrByVersionId_entries,
	sizeof(__CXMServer_QueryVersionTestItemArrByVersionId_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_GetFeedBack_struct
{
	FeedBack GetFeedBackResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_GetFeedBack_entries[] =
{

	{
		0xF2A533C4, 
		"GetFeedBackResult", 
		L"GetFeedBackResult", 
		sizeof("GetFeedBackResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_GetFeedBack_struct, GetFeedBackResult),
		NULL,
		&__FeedBack_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_GetFeedBack_map =
{
	0x30685514,
	"GetFeedBack",
	L"GetFeedBackResponse",
	sizeof("GetFeedBack")-1,
	sizeof("GetFeedBackResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_GetFeedBack_entries,
	sizeof(__CXMServer_GetFeedBack_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_UpdateVersion_TestItem_struct
{
	int sessionId;
	Version_testItem vt;
	bool UpdateVersion_TestItemResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_UpdateVersion_TestItem_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateVersion_TestItem_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000FAA, 
		"vt", 
		L"vt", 
		sizeof("vt")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateVersion_TestItem_struct, vt),
		NULL,
		&__Version_testItem_map,
		-1,
	},
	{
		0x256E4D56, 
		"UpdateVersion_TestItemResult", 
		L"UpdateVersion_TestItemResult", 
		sizeof("UpdateVersion_TestItemResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_UpdateVersion_TestItem_struct, UpdateVersion_TestItemResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_UpdateVersion_TestItem_map =
{
	0x39DE1B26,
	"UpdateVersion_TestItem",
	L"UpdateVersion_TestItemResponse",
	sizeof("UpdateVersion_TestItem")-1,
	sizeof("UpdateVersion_TestItemResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_UpdateVersion_TestItem_entries,
	sizeof(__CXMServer_UpdateVersion_TestItem_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_DownloadProblemGrade_struct
{
	int sessionId;
	int id;
	ProblemGrade DownloadProblemGradeResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_DownloadProblemGrade_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadProblemGrade_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadProblemGrade_struct, id),
		NULL,
		NULL,
		-1,
	},
	{
		0x6A05D04B, 
		"DownloadProblemGradeResult", 
		L"DownloadProblemGradeResult", 
		sizeof("DownloadProblemGradeResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_DownloadProblemGrade_struct, DownloadProblemGradeResult),
		NULL,
		&__ProblemGrade_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_DownloadProblemGrade_map =
{
	0x02622F5B,
	"DownloadProblemGrade",
	L"DownloadProblemGradeResponse",
	sizeof("DownloadProblemGrade")-1,
	sizeof("DownloadProblemGradeResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_DownloadProblemGrade_entries,
	sizeof(__CXMServer_DownloadProblemGrade_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_SetSqlLink_struct
{
	BSTR userName;
	BSTR password;
	BSTR linkStr;
	bool SetSqlLinkResult;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_SetSqlLink_entries[] =
{

	{
		0x8253F720, 
		"userName", 
		L"userName", 
		sizeof("userName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_SetSqlLink_struct, userName),
		NULL,
		NULL,
		-1,
	},
	{
		0x9C439233, 
		"password", 
		L"password", 
		sizeof("password")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_SetSqlLink_struct, password),
		NULL,
		NULL,
		-1,
	},
	{
		0x7681EFE7, 
		"linkStr", 
		L"linkStr", 
		sizeof("linkStr")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_SetSqlLink_struct, linkStr),
		NULL,
		NULL,
		-1,
	},
	{
		0x459680C9, 
		"SetSqlLinkResult", 
		L"SetSqlLinkResult", 
		sizeof("SetSqlLinkResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_SetSqlLink_struct, SetSqlLinkResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_SetSqlLink_map =
{
	0x04E0F759,
	"SetSqlLink",
	L"SetSqlLinkResponse",
	sizeof("SetSqlLink")-1,
	sizeof("SetSqlLinkResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_SetSqlLink_entries,
	sizeof(__CXMServer_SetSqlLink_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CXMServer_QueryNewsArrByDate_struct
{
	int sessionId;
	BSTR startDate;
	BSTR endDate;
	News *QueryNewsArrByDateResult;
	int __QueryNewsArrByDateResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CXMServer_QueryNewsArrByDate_entries[] =
{

	{
		0x323513F1, 
		"sessionId", 
		L"sessionId", 
		sizeof("sessionId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryNewsArrByDate_struct, sessionId),
		NULL,
		NULL,
		-1,
	},
	{
		0x9ED2666C, 
		"startDate", 
		L"startDate", 
		sizeof("startDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryNewsArrByDate_struct, startDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x6689F575, 
		"endDate", 
		L"endDate", 
		sizeof("endDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CXMServer_QueryNewsArrByDate_struct, endDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x9DE2D3D0, 
		"QueryNewsArrByDateResult", 
		L"QueryNewsArrByDateResult", 
		sizeof("QueryNewsArrByDateResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CXMServer_QueryNewsArrByDate_struct, QueryNewsArrByDateResult),
		NULL,
		&__News_map,
		3+1,
	},
	{
		0x9DE2D3D0,
		"__QueryNewsArrByDateResult_nSizeIs",
		L"__QueryNewsArrByDateResult_nSizeIs",
		sizeof("__QueryNewsArrByDateResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CXMServer_QueryNewsArrByDate_struct, __QueryNewsArrByDateResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CXMServer_QueryNewsArrByDate_map =
{
	0xA18E2820,
	"QueryNewsArrByDate",
	L"QueryNewsArrByDateResponse",
	sizeof("QueryNewsArrByDate")-1,
	sizeof("QueryNewsArrByDateResponse")-1,
	SOAPMAP_FUNC,
	__CXMServer_QueryNewsArrByDate_entries,
	sizeof(__CXMServer_QueryNewsArrByDate_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};

extern __declspec(selectany) const _soapmap * __CXMServer_funcs[] =
{
	&__CXMServer_GetUser_map,
	&__CXMServer_DownloadNotification_map,
	&__CXMServer_SaveOrUpdateTaskDispatch_map,
	&__CXMServer_SaveOrUpdateDownloadItem_map,
	&__CXMServer_QueryFeedBackArrByProblemType_map,
	&__CXMServer_QueryProblemGradeArr_map,
	&__CXMServer_UpdateUser_map,
	&__CXMServer_DownLoadNews_map,
	&__CXMServer_QueryProductionArr_map,
	&__CXMServer_QueryAllRole_map,
	&__CXMServer_InsertUser_map,
	&__CXMServer_UpdateUserTask_map,
	&__CXMServer_QueryChildTaskDispatchsByGroupId_map,
	&__CXMServer_UpdateTestItemArr_map,
	&__CXMServer_GetVersionRevision_map,
	&__CXMServer_DeleteCategoryById_map,
	&__CXMServer_QueryVersionArr_map,
	&__CXMServer_QueryTestItemSetArrByProId_map,
	&__CXMServer_GetFileHeader_map,
	&__CXMServer_GetReply_map,
	&__CXMServer_ValidateUserPerm_map,
	&__CXMServer_UpdateTaskDispatchArr_map,
	&__CXMServer_SaveOrUpdateProduction_map,
	&__CXMServer_InsertFeedBackArr_map,
	&__CXMServer_DownloadProduction_map,
	&__CXMServer_DeleteWorkLog_map,
	&__CXMServer_QueryTestItemArrBySetId_map,
	&__CXMServer_QueryClientProductByClientId_map,
	&__CXMServer_QueryProductionByUserId_map,
	&__CXMServer_QueryTestItemArrByCondition_map,
	&__CXMServer_QueryproductionsByClientId_map,
	&__CXMServer_DownloadFeedBack_map,
	&__CXMServer_GetWorkLog_map,
	&__CXMServer_DeleteClientById_map,
	&__CXMServer_DeleteTaskDispatch_map,
	&__CXMServer_GetRole_map,
	&__CXMServer_QueryDownloadItemByClient_map,
	&__CXMServer_DeleteDownloadItem_map,
	&__CXMServer_DeleteClient_DownloadItemByItem_map,
	&__CXMServer_QueryEssentialFeedBacks_map,
	&__CXMServer_QueryFeedBackArrByProcessState_map,
	&__CXMServer_QueryCategoryArrByObjType_map,
	&__CXMServer_SaveOrUpdateJob_map,
	&__CXMServer_QueryFeedBacksToDataSet_map,
	&__CXMServer_SaveOrUpdateNotification_map,
	&__CXMServer_InsertUser_Role_map,
	&__CXMServer_SaveOrUpdateVersionRevision_map,
	&__CXMServer_QueryTestItemByWorkLogDate_map,
	&__CXMServer_QueryFeedBackArrByName_map,
	&__CXMServer_DeleteUser_map,
	&__CXMServer_SaveOrUpdateCategory_map,
	&__CXMServer_DownloadClientProductById_map,
	&__CXMServer_QueryLatestVersionRevisionArr_map,
	&__CXMServer_DeleteReply_map,
	&__CXMServer_QueryTaskDispatchs_map,
	&__CXMServer_DownloadAttendanceById_map,
	&__CXMServer_DownloadCategoryById_map,
	&__CXMServer_QueryDevelopLogArr_map,
	&__CXMServer_DownloadItemById_map,
	&__CXMServer_InsertTaskDispatchArr_map,
	&__CXMServer_DownLoadLasrVersionByProId_map,
	&__CXMServer_QueryClientByProvinceNum_map,
	&__CXMServer_DeleteWorkLogById_map,
	&__CXMServer_QueryFeedBackArrByProposerId_map,
	&__CXMServer_DownLoadJobById_map,
	&__CXMServer_SaveOrUpdateTestItem_map,
	&__CXMServer_QueryAllJobs_map,
	&__CXMServer_QueryPartBuildByPartItemId_map,
	&__CXMServer_QueryRolesByUserId_map,
	&__CXMServer_QueryFeedBackArrByProblemGrade_map,
	&__CXMServer_DeleteTestItemSet_map,
	&__CXMServer_QueryFeedBackArrByCondition_map,
	&__CXMServer_DownloadVersionRevision_map,
	&__CXMServer_DeleteAttendance_map,
	&__CXMServer_ValidateUserName_map,
	&__CXMServer_DeleteTestItemById_map,
	&__CXMServer_DownloadTaskDispatch_map,
	&__CXMServer_QueryFeedBacksCount_map,
	&__CXMServer_QueryInsideUsers_map,
	&__CXMServer_GetLoginStruct_map,
	&__CXMServer_QueryTestItemArr_map,
	&__CXMServer_DeleteNotification_map,
	&__CXMServer_SaveOrUpdateNews_map,
	&__CXMServer_QueryVersionRevisionArrByProductionId_map,
	&__CXMServer_QueryFeedBackArrByProductionId_map,
	&__CXMServer_DeleteDeveLop_testItemByDevelopLopId_map,
	&__CXMServer_DeleteClient_DownloadItemByClient_map,
	&__CXMServer_QueryAttendanceByDate_map,
	&__CXMServer_GetTaskDispatch_map,
	&__CXMServer_DeleteRole_map,
	&__CXMServer_UpdateVersion_TestItemArr_map,
	&__CXMServer_SaveOrUpdateClient_map,
	&__CXMServer_QueryAllDevelopLog_map,
	&__CXMServer_UpdateFeedBacksSerialNo_map,
	&__CXMServer_DownloadFileHeader_map,
	&__CXMServer_QueryLikeClients_map,
	&__CXMServer_QueryWorkLogByDateAndProId_map,
	&__CXMServer_DownloadPartBuildById_map,
	&__CXMServer_LogoutUser_map,
	&__CXMServer_QueryLastReplyByFeedBackId_map,
	&__CXMServer_DownLoadClientById_map,
	&__CXMServer_QueryGroupFeedBacks_map,
	&__CXMServer_SaveOrUpdateFeedbackGroup_map,
	&__CXMServer_QueryUserByRoleId_map,
	&__CXMServer_DeleteClient_ProductionByProduct_map,
	&__CXMServer_SaveOrUpdateFeedBack_map,
	&__CXMServer_DeleteUser_RoleByUserIdAndRoleId_map,
	&__CXMServer_SaveOrUpdateTestItemSet_map,
	&__CXMServer_ClearUserPassword_map,
	&__CXMServer_QueryAllAttendance_map,
	&__CXMServer_DeleteDeveLop_testItemByItemId_map,
	&__CXMServer_QueryWorkLogByUserId_map,
	&__CXMServer_QueryFeedBackArrBySubmitDate_map,
	&__CXMServer_DownloadTestItemById_map,
	&__CXMServer_ValidatePassword_map,
	&__CXMServer_QueryTestItemByWorkLogId_map,
	&__CXMServer_QueryNotificationArr_map,
	&__CXMServer_GetRelationObject_map,
	&__CXMServer_DeletePartBuildByPartItemId_map,
	&__CXMServer_DownLoadMemberRole_map,
	&__CXMServer_DeleteProblemGradeById_map,
	&__CXMServer_QueryAllDownloadItem_map,
	&__CXMServer_QueryAllNews_map,
	&__CXMServer_GetNotification_map,
	&__CXMServer_SaveOrUpdateDevelopLog_map,
	&__CXMServer_QueryClientArr_map,
	&__CXMServer_SaveOrUpdateWorkLog_map,
	&__CXMServer_QueryCreateTaskUsers_map,
	&__CXMServer_DeleteClient_ProductionByClient_map,
	&__CXMServer_SaveOrUpdateReply_map,
	&__CXMServer_DeleteProduction_map,
	&__CXMServer_GetProduction_map,
	&__CXMServer_DownLoadTechnoiogyRole_map,
	&__CXMServer_QueryFeedBackArrByVersionRevision_map,
	&__CXMServer_SaveOrUpdateAttendance_map,
	&__CXMServer_DeleteClientDownloadByClientId_map,
	&__CXMServer_SaveOrUpdateRole_map,
	&__CXMServer_QueryAllFeedBackArr_map,
	&__CXMServer_DeleteFeedBackGroup_map,
	&__CXMServer_SaveOrUpdatePartBuild_map,
	&__CXMServer_DeleteDevelopLopById_map,
	&__CXMServer_DownloadWorkLogById_map,
	&__CXMServer_QueryAttendanceArrByUserId_map,
	&__CXMServer_DeleteJobById_map,
	&__CXMServer_UpdateTasksSerialNo_map,
	&__CXMServer_QueryAllUser_map,
	&__CXMServer_canExecuteFuncOper_map,
	&__CXMServer_DeleteFeedBack_map,
	&__CXMServer_DeletePartBuildById_map,
	&__CXMServer_QueryTestItemByVersionId_map,
	&__CXMServer_QueryVersionUpdateInfoArr_map,
	&__CXMServer_QueryCategoryArr_map,
	&__CXMServer_QueryFeedBacks_map,
	&__CXMServer_InserClientDownloadItem_map,
	&__CXMServer_DeleteVersionRevision_map,
	&__CXMServer_UpdateWorklog_testItem_map,
	&__CXMServer_QueryTaskDispatchArrByReceiverId_map,
	&__CXMServer_DeleteNewsById_map,
	&__CXMServer_DownloadUser_map,
	&__CXMServer_LoginUser_map,
	&__CXMServer_QueryAllPartBuild_map,
	&__CXMServer_QueryVersionTestItemArrByVersionId_map,
	&__CXMServer_GetFeedBack_map,
	&__CXMServer_UpdateVersion_TestItem_map,
	&__CXMServer_DownloadProblemGrade_map,
	&__CXMServer_SetSqlLink_map,
	&__CXMServer_QueryNewsArrByDate_map,
	NULL
};

template <typename TClient>
inline HRESULT CXMServerT<TClient>::GetUser(
		User* GetUserResult
	)
{
    if ( GetUserResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_GetUser_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 0);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetUser\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*GetUserResult = __params.GetUserResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownloadNotification(
		int sessionId, 
		int id, 
		Notification* DownloadNotificationResult
	)
{
    if ( DownloadNotificationResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownloadNotification_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 1);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownloadNotification\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownloadNotificationResult = __params.DownloadNotificationResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateTaskDispatch(
		int sessionId, 
		TaskDispatch taskDispatch, 
		bool isFile, 
		int* SaveOrUpdateTaskDispatchResult
	)
{
    if ( SaveOrUpdateTaskDispatchResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateTaskDispatch_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.taskDispatch = taskDispatch;
	__params.isFile = isFile;

	__atlsoap_hr = SetClientStruct(&__params, 2);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateTaskDispatch\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateTaskDispatchResult = __params.SaveOrUpdateTaskDispatchResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateDownloadItem(
		int sessionId, 
		DownloadItem item, 
		int* SaveOrUpdateDownloadItemResult
	)
{
    if ( SaveOrUpdateDownloadItemResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateDownloadItem_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.item = item;

	__atlsoap_hr = SetClientStruct(&__params, 3);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateDownloadItem\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateDownloadItemResult = __params.SaveOrUpdateDownloadItemResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryFeedBackArrByProblemType(
		int sessionId, 
		int problemType, 
		FeedBack** QueryFeedBackArrByProblemTypeResult, int* __QueryFeedBackArrByProblemTypeResult_nSizeIs
	)
{
    if ( QueryFeedBackArrByProblemTypeResult == NULL )
		return E_POINTER;
    if( __QueryFeedBackArrByProblemTypeResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryFeedBackArrByProblemType_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.problemType = problemType;

	__atlsoap_hr = SetClientStruct(&__params, 4);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryFeedBackArrByProblemType\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryFeedBackArrByProblemTypeResult = __params.QueryFeedBackArrByProblemTypeResult;
	*__QueryFeedBackArrByProblemTypeResult_nSizeIs = __params.__QueryFeedBackArrByProblemTypeResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryProblemGradeArr(
		int sessionId, 
		ProblemGrade** QueryProblemGradeArrResult, int* __QueryProblemGradeArrResult_nSizeIs
	)
{
    if ( QueryProblemGradeArrResult == NULL )
		return E_POINTER;
    if( __QueryProblemGradeArrResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryProblemGradeArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 5);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryProblemGradeArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryProblemGradeArrResult = __params.QueryProblemGradeArrResult;
	*__QueryProblemGradeArrResult_nSizeIs = __params.__QueryProblemGradeArrResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::UpdateUser(
		int sessionId, 
		User user, 
		bool isRole, 
		bool isProduct, 
		bool* UpdateUserResult
	)
{
    if ( UpdateUserResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_UpdateUser_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.user = user;
	__params.isRole = isRole;
	__params.isProduct = isProduct;

	__atlsoap_hr = SetClientStruct(&__params, 6);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/UpdateUser\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*UpdateUserResult = __params.UpdateUserResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownLoadNews(
		int sessionId, 
		int id, 
		News* DownLoadNewsResult
	)
{
    if ( DownLoadNewsResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownLoadNews_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 7);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownLoadNews\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownLoadNewsResult = __params.DownLoadNewsResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryProductionArr(
		int sessionId, 
		Production** QueryProductionArrResult, int* __QueryProductionArrResult_nSizeIs
	)
{
    if ( QueryProductionArrResult == NULL )
		return E_POINTER;
    if( __QueryProductionArrResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryProductionArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 8);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryProductionArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryProductionArrResult = __params.QueryProductionArrResult;
	*__QueryProductionArrResult_nSizeIs = __params.__QueryProductionArrResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryAllRole(
		int sessionId, 
		Role** QueryAllRoleResult, int* __QueryAllRoleResult_nSizeIs
	)
{
    if ( QueryAllRoleResult == NULL )
		return E_POINTER;
    if( __QueryAllRoleResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryAllRole_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 9);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryAllRole\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryAllRoleResult = __params.QueryAllRoleResult;
	*__QueryAllRoleResult_nSizeIs = __params.__QueryAllRoleResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::InsertUser(
		int sessionId, 
		User newUser, 
		int* InsertUserResult
	)
{
    if ( InsertUserResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_InsertUser_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.newUser = newUser;

	__atlsoap_hr = SetClientStruct(&__params, 10);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/InsertUser\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*InsertUserResult = __params.InsertUserResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::UpdateUserTask(
		int sessionId, 
		int userId, 
		BSTR nowTask, 
		bool* UpdateUserTaskResult
	)
{
    if ( UpdateUserTaskResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_UpdateUserTask_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.userId = userId;
	__params.nowTask = nowTask;

	__atlsoap_hr = SetClientStruct(&__params, 11);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/UpdateUserTask\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*UpdateUserTaskResult = __params.UpdateUserTaskResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryChildTaskDispatchsByGroupId(
		int sessionId, 
		int groupId, 
		TaskDispatch** QueryChildTaskDispatchsByGroupIdResult, int* __QueryChildTaskDispatchsByGroupIdResult_nSizeIs
	)
{
    if ( QueryChildTaskDispatchsByGroupIdResult == NULL )
		return E_POINTER;
    if( __QueryChildTaskDispatchsByGroupIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryChildTaskDispatchsByGroupId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.groupId = groupId;

	__atlsoap_hr = SetClientStruct(&__params, 12);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryChildTaskDispatchsByGroupId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryChildTaskDispatchsByGroupIdResult = __params.QueryChildTaskDispatchsByGroupIdResult;
	*__QueryChildTaskDispatchsByGroupIdResult_nSizeIs = __params.__QueryChildTaskDispatchsByGroupIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::UpdateTestItemArr(
		int sessionId, 
		TestItem* tiArr, int __tiArr_nSizeIs, 
		bool* UpdateTestItemArrResult
	)
{
    if ( tiArr == NULL )
		return E_POINTER;
    if ( UpdateTestItemArrResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_UpdateTestItemArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.tiArr = tiArr;
	__params.__tiArr_nSizeIs = __tiArr_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 13);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/UpdateTestItemArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*UpdateTestItemArrResult = __params.UpdateTestItemArrResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::GetVersionRevision(
		VersionRevision* GetVersionRevisionResult
	)
{
    if ( GetVersionRevisionResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_GetVersionRevision_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 14);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetVersionRevision\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*GetVersionRevisionResult = __params.GetVersionRevisionResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteCategoryById(
		int sessionId, 
		int id, 
		bool* DeleteCategoryByIdResult
	)
{
    if ( DeleteCategoryByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteCategoryById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 15);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteCategoryById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteCategoryByIdResult = __params.DeleteCategoryByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryVersionArr(
		int sessionId, 
		int productionId, 
		BSTR startDate, 
		BSTR endDate, 
		BSTR keyword, 
		VersionRevision** QueryVersionArrResult, int* __QueryVersionArrResult_nSizeIs
	)
{
    if ( QueryVersionArrResult == NULL )
		return E_POINTER;
    if( __QueryVersionArrResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryVersionArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.productionId = productionId;
	__params.startDate = startDate;
	__params.endDate = endDate;
	__params.keyword = keyword;

	__atlsoap_hr = SetClientStruct(&__params, 16);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryVersionArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryVersionArrResult = __params.QueryVersionArrResult;
	*__QueryVersionArrResult_nSizeIs = __params.__QueryVersionArrResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryTestItemSetArrByProId(
		int sessionId, 
		int proId, 
		TestItemSet** QueryTestItemSetArrByProIdResult, int* __QueryTestItemSetArrByProIdResult_nSizeIs
	)
{
    if ( QueryTestItemSetArrByProIdResult == NULL )
		return E_POINTER;
    if( __QueryTestItemSetArrByProIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryTestItemSetArrByProId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.proId = proId;

	__atlsoap_hr = SetClientStruct(&__params, 17);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryTestItemSetArrByProId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryTestItemSetArrByProIdResult = __params.QueryTestItemSetArrByProIdResult;
	*__QueryTestItemSetArrByProIdResult_nSizeIs = __params.__QueryTestItemSetArrByProIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::GetFileHeader(
		FileHeader* GetFileHeaderResult
	)
{
    if ( GetFileHeaderResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_GetFileHeader_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 18);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetFileHeader\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*GetFileHeaderResult = __params.GetFileHeaderResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::GetReply(
		Reply* GetReplyResult
	)
{
    if ( GetReplyResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_GetReply_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 19);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetReply\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*GetReplyResult = __params.GetReplyResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::ValidateUserPerm(
		int sessionId, 
		int permOper, 
		bool* ValidateUserPermResult
	)
{
    if ( ValidateUserPermResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_ValidateUserPerm_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.permOper = permOper;

	__atlsoap_hr = SetClientStruct(&__params, 20);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/ValidateUserPerm\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*ValidateUserPermResult = __params.ValidateUserPermResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::UpdateTaskDispatchArr(
		int sessionId, 
		TaskDispatch* taskArr, int __taskArr_nSizeIs, 
		int** UpdateTaskDispatchArrResult, int* __UpdateTaskDispatchArrResult_nSizeIs
	)
{
    if ( taskArr == NULL )
		return E_POINTER;
    if ( UpdateTaskDispatchArrResult == NULL )
		return E_POINTER;
    if( __UpdateTaskDispatchArrResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_UpdateTaskDispatchArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.taskArr = taskArr;
	__params.__taskArr_nSizeIs = __taskArr_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 21);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/UpdateTaskDispatchArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*UpdateTaskDispatchArrResult = __params.UpdateTaskDispatchArrResult;
	*__UpdateTaskDispatchArrResult_nSizeIs = __params.__UpdateTaskDispatchArrResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateProduction(
		int sessionId, 
		Production product, 
		int* SaveOrUpdateProductionResult
	)
{
    if ( SaveOrUpdateProductionResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateProduction_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.product = product;

	__atlsoap_hr = SetClientStruct(&__params, 22);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateProduction\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateProductionResult = __params.SaveOrUpdateProductionResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::InsertFeedBackArr(
		int sessionId, 
		FeedBack* feedBackArr, int __feedBackArr_nSizeIs, 
		bool* InsertFeedBackArrResult
	)
{
    if ( feedBackArr == NULL )
		return E_POINTER;
    if ( InsertFeedBackArrResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_InsertFeedBackArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.feedBackArr = feedBackArr;
	__params.__feedBackArr_nSizeIs = __feedBackArr_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 23);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/InsertFeedBackArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*InsertFeedBackArrResult = __params.InsertFeedBackArrResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownloadProduction(
		int sessionId, 
		int id, 
		Production* DownloadProductionResult
	)
{
    if ( DownloadProductionResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownloadProduction_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 24);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownloadProduction\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownloadProductionResult = __params.DownloadProductionResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteWorkLog(
		int sessionId, 
		int logId, 
		bool* DeleteWorkLogResult
	)
{
    if ( DeleteWorkLogResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteWorkLog_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.logId = logId;

	__atlsoap_hr = SetClientStruct(&__params, 25);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteWorkLog\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteWorkLogResult = __params.DeleteWorkLogResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryTestItemArrBySetId(
		int sessionId, 
		int setId, 
		bool share, 
		TestItem** QueryTestItemArrBySetIdResult, int* __QueryTestItemArrBySetIdResult_nSizeIs
	)
{
    if ( QueryTestItemArrBySetIdResult == NULL )
		return E_POINTER;
    if( __QueryTestItemArrBySetIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryTestItemArrBySetId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.setId = setId;
	__params.share = share;

	__atlsoap_hr = SetClientStruct(&__params, 26);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryTestItemArrBySetId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryTestItemArrBySetIdResult = __params.QueryTestItemArrBySetIdResult;
	*__QueryTestItemArrBySetIdResult_nSizeIs = __params.__QueryTestItemArrBySetIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryClientProductByClientId(
		int sessionId, 
		int clientId, 
		Client_Production** QueryClientProductByClientIdResult, int* __QueryClientProductByClientIdResult_nSizeIs
	)
{
    if ( QueryClientProductByClientIdResult == NULL )
		return E_POINTER;
    if( __QueryClientProductByClientIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryClientProductByClientId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.clientId = clientId;

	__atlsoap_hr = SetClientStruct(&__params, 27);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryClientProductByClientId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryClientProductByClientIdResult = __params.QueryClientProductByClientIdResult;
	*__QueryClientProductByClientIdResult_nSizeIs = __params.__QueryClientProductByClientIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryProductionByUserId(
		int sessionId, 
		int userId, 
		Production** QueryProductionByUserIdResult, int* __QueryProductionByUserIdResult_nSizeIs
	)
{
    if ( QueryProductionByUserIdResult == NULL )
		return E_POINTER;
    if( __QueryProductionByUserIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryProductionByUserId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.userId = userId;

	__atlsoap_hr = SetClientStruct(&__params, 28);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryProductionByUserId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryProductionByUserIdResult = __params.QueryProductionByUserIdResult;
	*__QueryProductionByUserIdResult_nSizeIs = __params.__QueryProductionByUserIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryTestItemArrByCondition(
		int sessionId, 
		int proId, 
		int categoryId, 
		bool share, 
		TestItem** QueryTestItemArrByConditionResult, int* __QueryTestItemArrByConditionResult_nSizeIs
	)
{
    if ( QueryTestItemArrByConditionResult == NULL )
		return E_POINTER;
    if( __QueryTestItemArrByConditionResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryTestItemArrByCondition_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.proId = proId;
	__params.categoryId = categoryId;
	__params.share = share;

	__atlsoap_hr = SetClientStruct(&__params, 29);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryTestItemArrByCondition\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryTestItemArrByConditionResult = __params.QueryTestItemArrByConditionResult;
	*__QueryTestItemArrByConditionResult_nSizeIs = __params.__QueryTestItemArrByConditionResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryproductionsByClientId(
		int sessionId, 
		int clientId, 
		Production** QueryproductionsByClientIdResult, int* __QueryproductionsByClientIdResult_nSizeIs
	)
{
    if ( QueryproductionsByClientIdResult == NULL )
		return E_POINTER;
    if( __QueryproductionsByClientIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryproductionsByClientId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.clientId = clientId;

	__atlsoap_hr = SetClientStruct(&__params, 30);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryproductionsByClientId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryproductionsByClientIdResult = __params.QueryproductionsByClientIdResult;
	*__QueryproductionsByClientIdResult_nSizeIs = __params.__QueryproductionsByClientIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownloadFeedBack(
		int sessionId, 
		int id, 
		FeedBack* DownloadFeedBackResult
	)
{
    if ( DownloadFeedBackResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownloadFeedBack_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 31);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownloadFeedBack\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownloadFeedBackResult = __params.DownloadFeedBackResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::GetWorkLog(
		WorkLog* GetWorkLogResult
	)
{
    if ( GetWorkLogResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_GetWorkLog_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 32);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetWorkLog\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*GetWorkLogResult = __params.GetWorkLogResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteClientById(
		int sessionId, 
		int id, 
		bool* DeleteClientByIdResult
	)
{
    if ( DeleteClientByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteClientById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 33);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteClientById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteClientByIdResult = __params.DeleteClientByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteTaskDispatch(
		int sessionId, 
		int id, 
		bool isGroup, 
		bool* DeleteTaskDispatchResult
	)
{
    if ( DeleteTaskDispatchResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteTaskDispatch_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;
	__params.isGroup = isGroup;

	__atlsoap_hr = SetClientStruct(&__params, 34);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteTaskDispatch\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteTaskDispatchResult = __params.DeleteTaskDispatchResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::GetRole(
		Role* GetRoleResult
	)
{
    if ( GetRoleResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_GetRole_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 35);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetRole\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*GetRoleResult = __params.GetRoleResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryDownloadItemByClient(
		int sessionId, 
		int clientId, 
		DownloadItem** QueryDownloadItemByClientResult, int* __QueryDownloadItemByClientResult_nSizeIs
	)
{
    if ( QueryDownloadItemByClientResult == NULL )
		return E_POINTER;
    if( __QueryDownloadItemByClientResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryDownloadItemByClient_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.clientId = clientId;

	__atlsoap_hr = SetClientStruct(&__params, 36);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryDownloadItemByClient\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryDownloadItemByClientResult = __params.QueryDownloadItemByClientResult;
	*__QueryDownloadItemByClientResult_nSizeIs = __params.__QueryDownloadItemByClientResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteDownloadItem(
		int sessionId, 
		int id, 
		bool* DeleteDownloadItemResult
	)
{
    if ( DeleteDownloadItemResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteDownloadItem_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 37);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteDownloadItem\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteDownloadItemResult = __params.DeleteDownloadItemResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteClient_DownloadItemByItem(
		int itemId, 
		bool* DeleteClient_DownloadItemByItemResult
	)
{
    if ( DeleteClient_DownloadItemByItemResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteClient_DownloadItemByItem_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.itemId = itemId;

	__atlsoap_hr = SetClientStruct(&__params, 38);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteClient_DownloadItemByItem\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteClient_DownloadItemByItemResult = __params.DeleteClient_DownloadItemByItemResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryEssentialFeedBacks(
		int productId, 
		FeedBack** QueryEssentialFeedBacksResult, int* __QueryEssentialFeedBacksResult_nSizeIs
	)
{
    if ( QueryEssentialFeedBacksResult == NULL )
		return E_POINTER;
    if( __QueryEssentialFeedBacksResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryEssentialFeedBacks_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.productId = productId;

	__atlsoap_hr = SetClientStruct(&__params, 39);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryEssentialFeedBacks\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryEssentialFeedBacksResult = __params.QueryEssentialFeedBacksResult;
	*__QueryEssentialFeedBacksResult_nSizeIs = __params.__QueryEssentialFeedBacksResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryFeedBackArrByProcessState(
		int sessionId, 
		bool state, 
		FeedBack** QueryFeedBackArrByProcessStateResult, int* __QueryFeedBackArrByProcessStateResult_nSizeIs
	)
{
    if ( QueryFeedBackArrByProcessStateResult == NULL )
		return E_POINTER;
    if( __QueryFeedBackArrByProcessStateResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryFeedBackArrByProcessState_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.state = state;

	__atlsoap_hr = SetClientStruct(&__params, 40);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryFeedBackArrByProcessState\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryFeedBackArrByProcessStateResult = __params.QueryFeedBackArrByProcessStateResult;
	*__QueryFeedBackArrByProcessStateResult_nSizeIs = __params.__QueryFeedBackArrByProcessStateResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryCategoryArrByObjType(
		int sessionId, 
		int objType, 
		Category** QueryCategoryArrByObjTypeResult, int* __QueryCategoryArrByObjTypeResult_nSizeIs
	)
{
    if ( QueryCategoryArrByObjTypeResult == NULL )
		return E_POINTER;
    if( __QueryCategoryArrByObjTypeResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryCategoryArrByObjType_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.objType = objType;

	__atlsoap_hr = SetClientStruct(&__params, 41);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryCategoryArrByObjType\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryCategoryArrByObjTypeResult = __params.QueryCategoryArrByObjTypeResult;
	*__QueryCategoryArrByObjTypeResult_nSizeIs = __params.__QueryCategoryArrByObjTypeResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateJob(
		int sessionId, 
		Job job, 
		int* SaveOrUpdateJobResult
	)
{
    if ( SaveOrUpdateJobResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateJob_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.job = job;

	__atlsoap_hr = SetClientStruct(&__params, 42);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateJob\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateJobResult = __params.SaveOrUpdateJobResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryFeedBacksToDataSet(
		int sessionId, 
		int promulgatorId, 
		bool isUnSolve, 
		bool isSolve, 
		bool isGroup, 
		BSTR* QueryFeedBacksToDataSetResult
	)
{
    if ( QueryFeedBacksToDataSetResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryFeedBacksToDataSet_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.promulgatorId = promulgatorId;
	__params.isUnSolve = isUnSolve;
	__params.isSolve = isSolve;
	__params.isGroup = isGroup;

	__atlsoap_hr = SetClientStruct(&__params, 43);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryFeedBacksToDataSet\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryFeedBacksToDataSetResult = __params.QueryFeedBacksToDataSetResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateNotification(
		int sessionId, 
		Notification notification, 
		bool isFile, 
		int* SaveOrUpdateNotificationResult
	)
{
    if ( SaveOrUpdateNotificationResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateNotification_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.notification = notification;
	__params.isFile = isFile;

	__atlsoap_hr = SetClientStruct(&__params, 44);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateNotification\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateNotificationResult = __params.SaveOrUpdateNotificationResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::InsertUser_Role(
		int sessionId, 
		int userId, 
		Role* roleArr, int __roleArr_nSizeIs, 
		bool* InsertUser_RoleResult
	)
{
    if ( roleArr == NULL )
		return E_POINTER;
    if ( InsertUser_RoleResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_InsertUser_Role_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.userId = userId;
	__params.roleArr = roleArr;
	__params.__roleArr_nSizeIs = __roleArr_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 45);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/InsertUser_Role\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*InsertUser_RoleResult = __params.InsertUser_RoleResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateVersionRevision(
		int sessionId, 
		VersionRevision version, 
		int* SaveOrUpdateVersionRevisionResult
	)
{
    if ( SaveOrUpdateVersionRevisionResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateVersionRevision_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.version = version;

	__atlsoap_hr = SetClientStruct(&__params, 46);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateVersionRevision\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateVersionRevisionResult = __params.SaveOrUpdateVersionRevisionResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryTestItemByWorkLogDate(
		int sessionId, 
		BSTR startDate, 
		BSTR endDate, 
		TestItem** QueryTestItemByWorkLogDateResult, int* __QueryTestItemByWorkLogDateResult_nSizeIs
	)
{
    if ( QueryTestItemByWorkLogDateResult == NULL )
		return E_POINTER;
    if( __QueryTestItemByWorkLogDateResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryTestItemByWorkLogDate_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.startDate = startDate;
	__params.endDate = endDate;

	__atlsoap_hr = SetClientStruct(&__params, 47);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryTestItemByWorkLogDate\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryTestItemByWorkLogDateResult = __params.QueryTestItemByWorkLogDateResult;
	*__QueryTestItemByWorkLogDateResult_nSizeIs = __params.__QueryTestItemByWorkLogDateResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryFeedBackArrByName(
		int sessionId, 
		BSTR name, 
		FeedBack** QueryFeedBackArrByNameResult, int* __QueryFeedBackArrByNameResult_nSizeIs
	)
{
    if ( QueryFeedBackArrByNameResult == NULL )
		return E_POINTER;
    if( __QueryFeedBackArrByNameResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryFeedBackArrByName_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.name = name;

	__atlsoap_hr = SetClientStruct(&__params, 48);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryFeedBackArrByName\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryFeedBackArrByNameResult = __params.QueryFeedBackArrByNameResult;
	*__QueryFeedBackArrByNameResult_nSizeIs = __params.__QueryFeedBackArrByNameResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteUser(
		int sessionId, 
		int id, 
		bool* DeleteUserResult
	)
{
    if ( DeleteUserResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteUser_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 49);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteUser\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteUserResult = __params.DeleteUserResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateCategory(
		int sessionId, 
		Category category, 
		int* SaveOrUpdateCategoryResult
	)
{
    if ( SaveOrUpdateCategoryResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateCategory_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.category = category;

	__atlsoap_hr = SetClientStruct(&__params, 50);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateCategory\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateCategoryResult = __params.SaveOrUpdateCategoryResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownloadClientProductById(
		int id, 
		Client_Production* DownloadClientProductByIdResult
	)
{
    if ( DownloadClientProductByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownloadClientProductById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 51);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownloadClientProductById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownloadClientProductByIdResult = __params.DownloadClientProductByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryLatestVersionRevisionArr(
		int sessionId, 
		VersionRevision** QueryLatestVersionRevisionArrResult, int* __QueryLatestVersionRevisionArrResult_nSizeIs
	)
{
    if ( QueryLatestVersionRevisionArrResult == NULL )
		return E_POINTER;
    if( __QueryLatestVersionRevisionArrResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryLatestVersionRevisionArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 52);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryLatestVersionRevisionArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryLatestVersionRevisionArrResult = __params.QueryLatestVersionRevisionArrResult;
	*__QueryLatestVersionRevisionArrResult_nSizeIs = __params.__QueryLatestVersionRevisionArrResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteReply(
		int sessionId, 
		int id, 
		bool* DeleteReplyResult
	)
{
    if ( DeleteReplyResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteReply_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 53);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteReply\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteReplyResult = __params.DeleteReplyResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryTaskDispatchs(
		int sessionId, 
		BSTR keyword, 
		int promulgatorId, 
		int acceptUserId, 
		int task_state, 
		BSTR startDate, 
		BSTR endDate, 
		int taskProId, 
		TaskDispatch** QueryTaskDispatchsResult, int* __QueryTaskDispatchsResult_nSizeIs
	)
{
    if ( QueryTaskDispatchsResult == NULL )
		return E_POINTER;
    if( __QueryTaskDispatchsResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryTaskDispatchs_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.keyword = keyword;
	__params.promulgatorId = promulgatorId;
	__params.acceptUserId = acceptUserId;
	__params.task_state = task_state;
	__params.startDate = startDate;
	__params.endDate = endDate;
	__params.taskProId = taskProId;

	__atlsoap_hr = SetClientStruct(&__params, 54);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryTaskDispatchs\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryTaskDispatchsResult = __params.QueryTaskDispatchsResult;
	*__QueryTaskDispatchsResult_nSizeIs = __params.__QueryTaskDispatchsResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownloadAttendanceById(
		int sessionId, 
		int id, 
		Attendance* DownloadAttendanceByIdResult
	)
{
    if ( DownloadAttendanceByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownloadAttendanceById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 55);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownloadAttendanceById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownloadAttendanceByIdResult = __params.DownloadAttendanceByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownloadCategoryById(
		int sessionId, 
		int id, 
		Category* DownloadCategoryByIdResult
	)
{
    if ( DownloadCategoryByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownloadCategoryById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 56);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownloadCategoryById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownloadCategoryByIdResult = __params.DownloadCategoryByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryDevelopLogArr(
		int sessionId, 
		int* productIdArr, int __productIdArr_nSizeIs, 
		int userId, 
		BSTR startDate, 
		BSTR endDate, 
		DevelopLog** QueryDevelopLogArrResult, int* __QueryDevelopLogArrResult_nSizeIs
	)
{
    if ( productIdArr == NULL )
		return E_POINTER;
    if ( QueryDevelopLogArrResult == NULL )
		return E_POINTER;
    if( __QueryDevelopLogArrResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryDevelopLogArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.productIdArr = productIdArr;
	__params.__productIdArr_nSizeIs = __productIdArr_nSizeIs;
	__params.userId = userId;
	__params.startDate = startDate;
	__params.endDate = endDate;

	__atlsoap_hr = SetClientStruct(&__params, 57);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryDevelopLogArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryDevelopLogArrResult = __params.QueryDevelopLogArrResult;
	*__QueryDevelopLogArrResult_nSizeIs = __params.__QueryDevelopLogArrResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownloadItemById(
		int id, 
		DownloadItem* DownloadItemByIdResult
	)
{
    if ( DownloadItemByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownloadItemById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 58);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownloadItemById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownloadItemByIdResult = __params.DownloadItemByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::InsertTaskDispatchArr(
		int sessionId, 
		TaskDispatch* taskArr, int __taskArr_nSizeIs, 
		int** InsertTaskDispatchArrResult, int* __InsertTaskDispatchArrResult_nSizeIs
	)
{
    if ( taskArr == NULL )
		return E_POINTER;
    if ( InsertTaskDispatchArrResult == NULL )
		return E_POINTER;
    if( __InsertTaskDispatchArrResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_InsertTaskDispatchArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.taskArr = taskArr;
	__params.__taskArr_nSizeIs = __taskArr_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 59);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/InsertTaskDispatchArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*InsertTaskDispatchArrResult = __params.InsertTaskDispatchArrResult;
	*__InsertTaskDispatchArrResult_nSizeIs = __params.__InsertTaskDispatchArrResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownLoadLasrVersionByProId(
		int sessionId, 
		int productionId, 
		VersionRevision* DownLoadLasrVersionByProIdResult
	)
{
    if ( DownLoadLasrVersionByProIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownLoadLasrVersionByProId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.productionId = productionId;

	__atlsoap_hr = SetClientStruct(&__params, 60);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownLoadLasrVersionByProId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownLoadLasrVersionByProIdResult = __params.DownLoadLasrVersionByProIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryClientByProvinceNum(
		int sessionId, 
		int num, 
		Client** QueryClientByProvinceNumResult, int* __QueryClientByProvinceNumResult_nSizeIs
	)
{
    if ( QueryClientByProvinceNumResult == NULL )
		return E_POINTER;
    if( __QueryClientByProvinceNumResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryClientByProvinceNum_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.num = num;

	__atlsoap_hr = SetClientStruct(&__params, 61);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryClientByProvinceNum\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryClientByProvinceNumResult = __params.QueryClientByProvinceNumResult;
	*__QueryClientByProvinceNumResult_nSizeIs = __params.__QueryClientByProvinceNumResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteWorkLogById(
		int sessionId, 
		int id, 
		bool* DeleteWorkLogByIdResult
	)
{
    if ( DeleteWorkLogByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteWorkLogById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 62);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteWorkLogById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteWorkLogByIdResult = __params.DeleteWorkLogByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryFeedBackArrByProposerId(
		int sessionId, 
		int proposerId, 
		FeedBack** QueryFeedBackArrByProposerIdResult, int* __QueryFeedBackArrByProposerIdResult_nSizeIs
	)
{
    if ( QueryFeedBackArrByProposerIdResult == NULL )
		return E_POINTER;
    if( __QueryFeedBackArrByProposerIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryFeedBackArrByProposerId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.proposerId = proposerId;

	__atlsoap_hr = SetClientStruct(&__params, 63);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryFeedBackArrByProposerId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryFeedBackArrByProposerIdResult = __params.QueryFeedBackArrByProposerIdResult;
	*__QueryFeedBackArrByProposerIdResult_nSizeIs = __params.__QueryFeedBackArrByProposerIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownLoadJobById(
		int sessionId, 
		int id, 
		Job* DownLoadJobByIdResult
	)
{
    if ( DownLoadJobByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownLoadJobById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 64);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownLoadJobById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownLoadJobByIdResult = __params.DownLoadJobByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateTestItem(
		int sessionId, 
		TestItem ti, 
		int* SaveOrUpdateTestItemResult
	)
{
    if ( SaveOrUpdateTestItemResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateTestItem_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.ti = ti;

	__atlsoap_hr = SetClientStruct(&__params, 65);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateTestItem\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateTestItemResult = __params.SaveOrUpdateTestItemResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryAllJobs(
		int sessionId, 
		Job** QueryAllJobsResult, int* __QueryAllJobsResult_nSizeIs
	)
{
    if ( QueryAllJobsResult == NULL )
		return E_POINTER;
    if( __QueryAllJobsResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryAllJobs_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 66);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryAllJobs\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryAllJobsResult = __params.QueryAllJobsResult;
	*__QueryAllJobsResult_nSizeIs = __params.__QueryAllJobsResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryPartBuildByPartItemId(
		int partItemId, 
		PartBuild** QueryPartBuildByPartItemIdResult, int* __QueryPartBuildByPartItemIdResult_nSizeIs
	)
{
    if ( QueryPartBuildByPartItemIdResult == NULL )
		return E_POINTER;
    if( __QueryPartBuildByPartItemIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryPartBuildByPartItemId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.partItemId = partItemId;

	__atlsoap_hr = SetClientStruct(&__params, 67);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryPartBuildByPartItemId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryPartBuildByPartItemIdResult = __params.QueryPartBuildByPartItemIdResult;
	*__QueryPartBuildByPartItemIdResult_nSizeIs = __params.__QueryPartBuildByPartItemIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryRolesByUserId(
		int userId, 
		Role** QueryRolesByUserIdResult, int* __QueryRolesByUserIdResult_nSizeIs
	)
{
    if ( QueryRolesByUserIdResult == NULL )
		return E_POINTER;
    if( __QueryRolesByUserIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryRolesByUserId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.userId = userId;

	__atlsoap_hr = SetClientStruct(&__params, 68);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryRolesByUserId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryRolesByUserIdResult = __params.QueryRolesByUserIdResult;
	*__QueryRolesByUserIdResult_nSizeIs = __params.__QueryRolesByUserIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryFeedBackArrByProblemGrade(
		int sessionId, 
		int problemGrade, 
		FeedBack** QueryFeedBackArrByProblemGradeResult, int* __QueryFeedBackArrByProblemGradeResult_nSizeIs
	)
{
    if ( QueryFeedBackArrByProblemGradeResult == NULL )
		return E_POINTER;
    if( __QueryFeedBackArrByProblemGradeResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryFeedBackArrByProblemGrade_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.problemGrade = problemGrade;

	__atlsoap_hr = SetClientStruct(&__params, 69);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryFeedBackArrByProblemGrade\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryFeedBackArrByProblemGradeResult = __params.QueryFeedBackArrByProblemGradeResult;
	*__QueryFeedBackArrByProblemGradeResult_nSizeIs = __params.__QueryFeedBackArrByProblemGradeResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteTestItemSet(
		int sessionId, 
		int id, 
		bool* DeleteTestItemSetResult
	)
{
    if ( DeleteTestItemSetResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteTestItemSet_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 70);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteTestItemSet\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteTestItemSetResult = __params.DeleteTestItemSetResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryFeedBackArrByCondition(
		int sessionId, 
		BSTR keyword, 
		int gradeId, 
		BSTR problemType, 
		BSTR processState, 
		int* productIdArr, int __productIdArr_nSizeIs, 
		FeedBack** QueryFeedBackArrByConditionResult, int* __QueryFeedBackArrByConditionResult_nSizeIs
	)
{
    if ( productIdArr == NULL )
		return E_POINTER;
    if ( QueryFeedBackArrByConditionResult == NULL )
		return E_POINTER;
    if( __QueryFeedBackArrByConditionResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryFeedBackArrByCondition_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.keyword = keyword;
	__params.gradeId = gradeId;
	__params.problemType = problemType;
	__params.processState = processState;
	__params.productIdArr = productIdArr;
	__params.__productIdArr_nSizeIs = __productIdArr_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 71);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryFeedBackArrByCondition\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryFeedBackArrByConditionResult = __params.QueryFeedBackArrByConditionResult;
	*__QueryFeedBackArrByConditionResult_nSizeIs = __params.__QueryFeedBackArrByConditionResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownloadVersionRevision(
		int sessionId, 
		int id, 
		VersionRevision* DownloadVersionRevisionResult
	)
{
    if ( DownloadVersionRevisionResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownloadVersionRevision_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 72);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownloadVersionRevision\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownloadVersionRevisionResult = __params.DownloadVersionRevisionResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteAttendance(
		int sessionId, 
		int id, 
		bool* DeleteAttendanceResult
	)
{
    if ( DeleteAttendanceResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteAttendance_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 73);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteAttendance\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteAttendanceResult = __params.DeleteAttendanceResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::ValidateUserName(
		BSTR userName, 
		bool* ValidateUserNameResult
	)
{
    if ( ValidateUserNameResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_ValidateUserName_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.userName = userName;

	__atlsoap_hr = SetClientStruct(&__params, 74);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/ValidateUserName\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*ValidateUserNameResult = __params.ValidateUserNameResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteTestItemById(
		int sessionId, 
		int id, 
		bool* DeleteTestItemByIdResult
	)
{
    if ( DeleteTestItemByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteTestItemById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 75);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteTestItemById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteTestItemByIdResult = __params.DeleteTestItemByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownloadTaskDispatch(
		int sessionId, 
		int id, 
		TaskDispatch* DownloadTaskDispatchResult
	)
{
    if ( DownloadTaskDispatchResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownloadTaskDispatch_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 76);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownloadTaskDispatch\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownloadTaskDispatchResult = __params.DownloadTaskDispatchResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryFeedBacksCount(
		int sessionId, 
		int gradeId, 
		BSTR problemType, 
		BSTR processState, 
		int* productIdArr, int __productIdArr_nSizeIs, 
		int* QueryFeedBacksCountResult
	)
{
    if ( productIdArr == NULL )
		return E_POINTER;
    if ( QueryFeedBacksCountResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryFeedBacksCount_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.gradeId = gradeId;
	__params.problemType = problemType;
	__params.processState = processState;
	__params.productIdArr = productIdArr;
	__params.__productIdArr_nSizeIs = __productIdArr_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 77);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryFeedBacksCount\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryFeedBacksCountResult = __params.QueryFeedBacksCountResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryInsideUsers(
		int sessionId, 
		User** QueryInsideUsersResult, int* __QueryInsideUsersResult_nSizeIs
	)
{
    if ( QueryInsideUsersResult == NULL )
		return E_POINTER;
    if( __QueryInsideUsersResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryInsideUsers_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 78);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryInsideUsers\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryInsideUsersResult = __params.QueryInsideUsersResult;
	*__QueryInsideUsersResult_nSizeIs = __params.__QueryInsideUsersResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::GetLoginStruct(
		LoginStruct* GetLoginStructResult
	)
{
    if ( GetLoginStructResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_GetLoginStruct_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 79);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetLoginStruct\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*GetLoginStructResult = __params.GetLoginStructResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryTestItemArr(
		int sessionId, 
		TestItem** QueryTestItemArrResult, int* __QueryTestItemArrResult_nSizeIs
	)
{
    if ( QueryTestItemArrResult == NULL )
		return E_POINTER;
    if( __QueryTestItemArrResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryTestItemArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 80);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryTestItemArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryTestItemArrResult = __params.QueryTestItemArrResult;
	*__QueryTestItemArrResult_nSizeIs = __params.__QueryTestItemArrResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteNotification(
		int sessionId, 
		int id, 
		bool* DeleteNotificationResult
	)
{
    if ( DeleteNotificationResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteNotification_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 81);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteNotification\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteNotificationResult = __params.DeleteNotificationResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateNews(
		int sessionId, 
		News news, 
		int* SaveOrUpdateNewsResult
	)
{
    if ( SaveOrUpdateNewsResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateNews_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.news = news;

	__atlsoap_hr = SetClientStruct(&__params, 82);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateNews\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateNewsResult = __params.SaveOrUpdateNewsResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryVersionRevisionArrByProductionId(
		int sessionId, 
		int productionId, 
		VersionRevision** QueryVersionRevisionArrByProductionIdResult, int* __QueryVersionRevisionArrByProductionIdResult_nSizeIs
	)
{
    if ( QueryVersionRevisionArrByProductionIdResult == NULL )
		return E_POINTER;
    if( __QueryVersionRevisionArrByProductionIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryVersionRevisionArrByProductionId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.productionId = productionId;

	__atlsoap_hr = SetClientStruct(&__params, 83);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryVersionRevisionArrByProductionId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryVersionRevisionArrByProductionIdResult = __params.QueryVersionRevisionArrByProductionIdResult;
	*__QueryVersionRevisionArrByProductionIdResult_nSizeIs = __params.__QueryVersionRevisionArrByProductionIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryFeedBackArrByProductionId(
		int sessionId, 
		int productionId, 
		FeedBack** QueryFeedBackArrByProductionIdResult, int* __QueryFeedBackArrByProductionIdResult_nSizeIs
	)
{
    if ( QueryFeedBackArrByProductionIdResult == NULL )
		return E_POINTER;
    if( __QueryFeedBackArrByProductionIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryFeedBackArrByProductionId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.productionId = productionId;

	__atlsoap_hr = SetClientStruct(&__params, 84);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryFeedBackArrByProductionId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryFeedBackArrByProductionIdResult = __params.QueryFeedBackArrByProductionIdResult;
	*__QueryFeedBackArrByProductionIdResult_nSizeIs = __params.__QueryFeedBackArrByProductionIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteDeveLop_testItemByDevelopLopId(
		int sessionId, 
		int developLogId, 
		bool* DeleteDeveLop_testItemByDevelopLopIdResult
	)
{
    if ( DeleteDeveLop_testItemByDevelopLopIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteDeveLop_testItemByDevelopLopId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.developLogId = developLogId;

	__atlsoap_hr = SetClientStruct(&__params, 85);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteDeveLop_testItemByDevelopLopId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteDeveLop_testItemByDevelopLopIdResult = __params.DeleteDeveLop_testItemByDevelopLopIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteClient_DownloadItemByClient(
		int clientId, 
		bool* DeleteClient_DownloadItemByClientResult
	)
{
    if ( DeleteClient_DownloadItemByClientResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteClient_DownloadItemByClient_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.clientId = clientId;

	__atlsoap_hr = SetClientStruct(&__params, 86);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteClient_DownloadItemByClient\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteClient_DownloadItemByClientResult = __params.DeleteClient_DownloadItemByClientResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryAttendanceByDate(
		int sessionId, 
		BSTR startDate, 
		BSTR endDate, 
		Attendance** QueryAttendanceByDateResult, int* __QueryAttendanceByDateResult_nSizeIs
	)
{
    if ( QueryAttendanceByDateResult == NULL )
		return E_POINTER;
    if( __QueryAttendanceByDateResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryAttendanceByDate_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.startDate = startDate;
	__params.endDate = endDate;

	__atlsoap_hr = SetClientStruct(&__params, 87);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryAttendanceByDate\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryAttendanceByDateResult = __params.QueryAttendanceByDateResult;
	*__QueryAttendanceByDateResult_nSizeIs = __params.__QueryAttendanceByDateResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::GetTaskDispatch(
		TaskDispatch* GetTaskDispatchResult
	)
{
    if ( GetTaskDispatchResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_GetTaskDispatch_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 88);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetTaskDispatch\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*GetTaskDispatchResult = __params.GetTaskDispatchResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteRole(
		int sessionId, 
		int id, 
		bool* DeleteRoleResult
	)
{
    if ( DeleteRoleResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteRole_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 89);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteRole\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteRoleResult = __params.DeleteRoleResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::UpdateVersion_TestItemArr(
		int sessionId, 
		Version_testItem* vtArr, int __vtArr_nSizeIs, 
		bool* UpdateVersion_TestItemArrResult
	)
{
    if ( vtArr == NULL )
		return E_POINTER;
    if ( UpdateVersion_TestItemArrResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_UpdateVersion_TestItemArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.vtArr = vtArr;
	__params.__vtArr_nSizeIs = __vtArr_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 90);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/UpdateVersion_TestItemArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*UpdateVersion_TestItemArrResult = __params.UpdateVersion_TestItemArrResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateClient(
		int sessionId, 
		Client client, 
		bool isProduct, 
		bool isDownloadItem, 
		int* SaveOrUpdateClientResult
	)
{
    if ( SaveOrUpdateClientResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateClient_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.client = client;
	__params.isProduct = isProduct;
	__params.isDownloadItem = isDownloadItem;

	__atlsoap_hr = SetClientStruct(&__params, 91);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateClient\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateClientResult = __params.SaveOrUpdateClientResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryAllDevelopLog(
		int sessionId, 
		DevelopLog** QueryAllDevelopLogResult, int* __QueryAllDevelopLogResult_nSizeIs
	)
{
    if ( QueryAllDevelopLogResult == NULL )
		return E_POINTER;
    if( __QueryAllDevelopLogResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryAllDevelopLog_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 92);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryAllDevelopLog\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryAllDevelopLogResult = __params.QueryAllDevelopLogResult;
	*__QueryAllDevelopLogResult_nSizeIs = __params.__QueryAllDevelopLogResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::UpdateFeedBacksSerialNo(
		int sessionId, 
		int* idArr, int __idArr_nSizeIs, 
		int* serialArr, int __serialArr_nSizeIs, 
		bool* UpdateFeedBacksSerialNoResult
	)
{
    if ( idArr == NULL )
		return E_POINTER;
    if ( serialArr == NULL )
		return E_POINTER;
    if ( UpdateFeedBacksSerialNoResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_UpdateFeedBacksSerialNo_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.idArr = idArr;
	__params.__idArr_nSizeIs = __idArr_nSizeIs;
	__params.serialArr = serialArr;
	__params.__serialArr_nSizeIs = __serialArr_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 93);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/UpdateFeedBacksSerialNo\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*UpdateFeedBacksSerialNoResult = __params.UpdateFeedBacksSerialNoResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownloadFileHeader(
		int sessionId, 
		int id, 
		FileHeader* DownloadFileHeaderResult
	)
{
    if ( DownloadFileHeaderResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownloadFileHeader_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 94);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownloadFileHeader\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownloadFileHeaderResult = __params.DownloadFileHeaderResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryLikeClients(
		int sessionId, 
		BSTR keyword, 
		bool isProduct, 
		bool isDownloadItem, 
		Client** QueryLikeClientsResult, int* __QueryLikeClientsResult_nSizeIs
	)
{
    if ( QueryLikeClientsResult == NULL )
		return E_POINTER;
    if( __QueryLikeClientsResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryLikeClients_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.keyword = keyword;
	__params.isProduct = isProduct;
	__params.isDownloadItem = isDownloadItem;

	__atlsoap_hr = SetClientStruct(&__params, 95);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryLikeClients\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryLikeClientsResult = __params.QueryLikeClientsResult;
	*__QueryLikeClientsResult_nSizeIs = __params.__QueryLikeClientsResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryWorkLogByDateAndProId(
		int sessionId, 
		BSTR startDate, 
		BSTR endDate, 
		int proId, 
		WorkLog** QueryWorkLogByDateAndProIdResult, int* __QueryWorkLogByDateAndProIdResult_nSizeIs
	)
{
    if ( QueryWorkLogByDateAndProIdResult == NULL )
		return E_POINTER;
    if( __QueryWorkLogByDateAndProIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryWorkLogByDateAndProId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.startDate = startDate;
	__params.endDate = endDate;
	__params.proId = proId;

	__atlsoap_hr = SetClientStruct(&__params, 96);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryWorkLogByDateAndProId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryWorkLogByDateAndProIdResult = __params.QueryWorkLogByDateAndProIdResult;
	*__QueryWorkLogByDateAndProIdResult_nSizeIs = __params.__QueryWorkLogByDateAndProIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownloadPartBuildById(
		int id, 
		PartBuild* DownloadPartBuildByIdResult
	)
{
    if ( DownloadPartBuildByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownloadPartBuildById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 97);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownloadPartBuildById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownloadPartBuildByIdResult = __params.DownloadPartBuildByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::LogoutUser(
		int sessionId, 
		bool* LogoutUserResult
	)
{
    if ( LogoutUserResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_LogoutUser_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 98);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/LogoutUser\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*LogoutUserResult = __params.LogoutUserResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryLastReplyByFeedBackId(
		int sessionId, 
		int feedBackId, 
		Reply* QueryLastReplyByFeedBackIdResult
	)
{
    if ( QueryLastReplyByFeedBackIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryLastReplyByFeedBackId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.feedBackId = feedBackId;

	__atlsoap_hr = SetClientStruct(&__params, 99);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryLastReplyByFeedBackId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryLastReplyByFeedBackIdResult = __params.QueryLastReplyByFeedBackIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownLoadClientById(
		int sessionId, 
		int id, 
		Client* DownLoadClientByIdResult
	)
{
    if ( DownLoadClientByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownLoadClientById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 100);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownLoadClientById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownLoadClientByIdResult = __params.DownLoadClientByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryGroupFeedBacks(
		int sessionId, 
		int superiorId, 
		FeedBack** QueryGroupFeedBacksResult, int* __QueryGroupFeedBacksResult_nSizeIs
	)
{
    if ( QueryGroupFeedBacksResult == NULL )
		return E_POINTER;
    if( __QueryGroupFeedBacksResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryGroupFeedBacks_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.superiorId = superiorId;

	__atlsoap_hr = SetClientStruct(&__params, 101);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryGroupFeedBacks\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryGroupFeedBacksResult = __params.QueryGroupFeedBacksResult;
	*__QueryGroupFeedBacksResult_nSizeIs = __params.__QueryGroupFeedBacksResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateFeedbackGroup(
		int sessionId, 
		FeedBack* feedbackArr, int __feedbackArr_nSizeIs, 
		int** SaveOrUpdateFeedbackGroupResult, int* __SaveOrUpdateFeedbackGroupResult_nSizeIs
	)
{
    if ( feedbackArr == NULL )
		return E_POINTER;
    if ( SaveOrUpdateFeedbackGroupResult == NULL )
		return E_POINTER;
    if( __SaveOrUpdateFeedbackGroupResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateFeedbackGroup_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.feedbackArr = feedbackArr;
	__params.__feedbackArr_nSizeIs = __feedbackArr_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 102);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateFeedbackGroup\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateFeedbackGroupResult = __params.SaveOrUpdateFeedbackGroupResult;
	*__SaveOrUpdateFeedbackGroupResult_nSizeIs = __params.__SaveOrUpdateFeedbackGroupResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryUserByRoleId(
		int sessionId, 
		int roleId, 
		User** QueryUserByRoleIdResult, int* __QueryUserByRoleIdResult_nSizeIs
	)
{
    if ( QueryUserByRoleIdResult == NULL )
		return E_POINTER;
    if( __QueryUserByRoleIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryUserByRoleId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.roleId = roleId;

	__atlsoap_hr = SetClientStruct(&__params, 103);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryUserByRoleId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryUserByRoleIdResult = __params.QueryUserByRoleIdResult;
	*__QueryUserByRoleIdResult_nSizeIs = __params.__QueryUserByRoleIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteClient_ProductionByProduct(
		int productId, 
		bool* DeleteClient_ProductionByProductResult
	)
{
    if ( DeleteClient_ProductionByProductResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteClient_ProductionByProduct_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.productId = productId;

	__atlsoap_hr = SetClientStruct(&__params, 104);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteClient_ProductionByProduct\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteClient_ProductionByProductResult = __params.DeleteClient_ProductionByProductResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateFeedBack(
		int sessionId, 
		FeedBack feedBack, 
		bool isFile, 
		bool isRevert, 
		int* SaveOrUpdateFeedBackResult
	)
{
    if ( SaveOrUpdateFeedBackResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateFeedBack_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.feedBack = feedBack;
	__params.isFile = isFile;
	__params.isRevert = isRevert;

	__atlsoap_hr = SetClientStruct(&__params, 105);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateFeedBack\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateFeedBackResult = __params.SaveOrUpdateFeedBackResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteUser_RoleByUserIdAndRoleId(
		int sessionId, 
		int userId, 
		int roleId, 
		bool* DeleteUser_RoleByUserIdAndRoleIdResult
	)
{
    if ( DeleteUser_RoleByUserIdAndRoleIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteUser_RoleByUserIdAndRoleId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.userId = userId;
	__params.roleId = roleId;

	__atlsoap_hr = SetClientStruct(&__params, 106);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteUser_RoleByUserIdAndRoleId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteUser_RoleByUserIdAndRoleIdResult = __params.DeleteUser_RoleByUserIdAndRoleIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateTestItemSet(
		int sessionId, 
		TestItemSet testSet, 
		int* tiIdArr, int __tiIdArr_nSizeIs, 
		int* SaveOrUpdateTestItemSetResult
	)
{
    if ( tiIdArr == NULL )
		return E_POINTER;
    if ( SaveOrUpdateTestItemSetResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateTestItemSet_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.testSet = testSet;
	__params.tiIdArr = tiIdArr;
	__params.__tiIdArr_nSizeIs = __tiIdArr_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 107);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateTestItemSet\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateTestItemSetResult = __params.SaveOrUpdateTestItemSetResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::ClearUserPassword(
		int userId, 
		bool* ClearUserPasswordResult
	)
{
    if ( ClearUserPasswordResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_ClearUserPassword_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.userId = userId;

	__atlsoap_hr = SetClientStruct(&__params, 108);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/ClearUserPassword\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*ClearUserPasswordResult = __params.ClearUserPasswordResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryAllAttendance(
		Attendance** QueryAllAttendanceResult, int* __QueryAllAttendanceResult_nSizeIs
	)
{
    if ( QueryAllAttendanceResult == NULL )
		return E_POINTER;
    if( __QueryAllAttendanceResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryAllAttendance_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 109);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryAllAttendance\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryAllAttendanceResult = __params.QueryAllAttendanceResult;
	*__QueryAllAttendanceResult_nSizeIs = __params.__QueryAllAttendanceResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteDeveLop_testItemByItemId(
		int sessionId, 
		int itemId, 
		bool* DeleteDeveLop_testItemByItemIdResult
	)
{
    if ( DeleteDeveLop_testItemByItemIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteDeveLop_testItemByItemId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.itemId = itemId;

	__atlsoap_hr = SetClientStruct(&__params, 110);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteDeveLop_testItemByItemId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteDeveLop_testItemByItemIdResult = __params.DeleteDeveLop_testItemByItemIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryWorkLogByUserId(
		int sessionId, 
		int userId, 
		WorkLog** QueryWorkLogByUserIdResult, int* __QueryWorkLogByUserIdResult_nSizeIs
	)
{
    if ( QueryWorkLogByUserIdResult == NULL )
		return E_POINTER;
    if( __QueryWorkLogByUserIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryWorkLogByUserId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.userId = userId;

	__atlsoap_hr = SetClientStruct(&__params, 111);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryWorkLogByUserId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryWorkLogByUserIdResult = __params.QueryWorkLogByUserIdResult;
	*__QueryWorkLogByUserIdResult_nSizeIs = __params.__QueryWorkLogByUserIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryFeedBackArrBySubmitDate(
		int sessionId, 
		BSTR startDate, 
		BSTR endDate, 
		FeedBack** QueryFeedBackArrBySubmitDateResult, int* __QueryFeedBackArrBySubmitDateResult_nSizeIs
	)
{
    if ( QueryFeedBackArrBySubmitDateResult == NULL )
		return E_POINTER;
    if( __QueryFeedBackArrBySubmitDateResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryFeedBackArrBySubmitDate_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.startDate = startDate;
	__params.endDate = endDate;

	__atlsoap_hr = SetClientStruct(&__params, 112);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryFeedBackArrBySubmitDate\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryFeedBackArrBySubmitDateResult = __params.QueryFeedBackArrBySubmitDateResult;
	*__QueryFeedBackArrBySubmitDateResult_nSizeIs = __params.__QueryFeedBackArrBySubmitDateResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownloadTestItemById(
		int sessionId, 
		int id, 
		TestItem* DownloadTestItemByIdResult
	)
{
    if ( DownloadTestItemByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownloadTestItemById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 113);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownloadTestItemById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownloadTestItemByIdResult = __params.DownloadTestItemByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::ValidatePassword(
		int sessionId, 
		int userId, 
		BSTR password, 
		bool* ValidatePasswordResult
	)
{
    if ( ValidatePasswordResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_ValidatePassword_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.userId = userId;
	__params.password = password;

	__atlsoap_hr = SetClientStruct(&__params, 114);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/ValidatePassword\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*ValidatePasswordResult = __params.ValidatePasswordResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryTestItemByWorkLogId(
		int sessionId, 
		int logId, 
		TestItem** QueryTestItemByWorkLogIdResult, int* __QueryTestItemByWorkLogIdResult_nSizeIs
	)
{
    if ( QueryTestItemByWorkLogIdResult == NULL )
		return E_POINTER;
    if( __QueryTestItemByWorkLogIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryTestItemByWorkLogId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.logId = logId;

	__atlsoap_hr = SetClientStruct(&__params, 115);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryTestItemByWorkLogId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryTestItemByWorkLogIdResult = __params.QueryTestItemByWorkLogIdResult;
	*__QueryTestItemByWorkLogIdResult_nSizeIs = __params.__QueryTestItemByWorkLogIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryNotificationArr(
		int sessionId, 
		BSTR startDate, 
		BSTR endDate, 
		Notification** QueryNotificationArrResult, int* __QueryNotificationArrResult_nSizeIs
	)
{
    if ( QueryNotificationArrResult == NULL )
		return E_POINTER;
    if( __QueryNotificationArrResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryNotificationArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.startDate = startDate;
	__params.endDate = endDate;

	__atlsoap_hr = SetClientStruct(&__params, 116);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryNotificationArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryNotificationArrResult = __params.QueryNotificationArrResult;
	*__QueryNotificationArrResult_nSizeIs = __params.__QueryNotificationArrResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::GetRelationObject(
		RelationObject* GetRelationObjectResult
	)
{
    if ( GetRelationObjectResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_GetRelationObject_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 117);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetRelationObject\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*GetRelationObjectResult = __params.GetRelationObjectResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeletePartBuildByPartItemId(
		int partItemId, 
		bool* DeletePartBuildByPartItemIdResult
	)
{
    if ( DeletePartBuildByPartItemIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeletePartBuildByPartItemId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.partItemId = partItemId;

	__atlsoap_hr = SetClientStruct(&__params, 118);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeletePartBuildByPartItemId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeletePartBuildByPartItemIdResult = __params.DeletePartBuildByPartItemIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownLoadMemberRole(
		int sessionId, 
		Role* DownLoadMemberRoleResult
	)
{
    if ( DownLoadMemberRoleResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownLoadMemberRole_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 119);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownLoadMemberRole\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownLoadMemberRoleResult = __params.DownLoadMemberRoleResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteProblemGradeById(
		int sessionId, 
		int id, 
		bool* DeleteProblemGradeByIdResult
	)
{
    if ( DeleteProblemGradeByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteProblemGradeById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 120);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteProblemGradeById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteProblemGradeByIdResult = __params.DeleteProblemGradeByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryAllDownloadItem(
		int sessionId, 
		DownloadItem** QueryAllDownloadItemResult, int* __QueryAllDownloadItemResult_nSizeIs
	)
{
    if ( QueryAllDownloadItemResult == NULL )
		return E_POINTER;
    if( __QueryAllDownloadItemResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryAllDownloadItem_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 121);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryAllDownloadItem\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryAllDownloadItemResult = __params.QueryAllDownloadItemResult;
	*__QueryAllDownloadItemResult_nSizeIs = __params.__QueryAllDownloadItemResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryAllNews(
		int sessionId, 
		News** QueryAllNewsResult, int* __QueryAllNewsResult_nSizeIs
	)
{
    if ( QueryAllNewsResult == NULL )
		return E_POINTER;
    if( __QueryAllNewsResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryAllNews_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 122);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryAllNews\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryAllNewsResult = __params.QueryAllNewsResult;
	*__QueryAllNewsResult_nSizeIs = __params.__QueryAllNewsResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::GetNotification(
		Notification* GetNotificationResult
	)
{
    if ( GetNotificationResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_GetNotification_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 123);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetNotification\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*GetNotificationResult = __params.GetNotificationResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateDevelopLog(
		int sessionId, 
		DevelopLog developLog, 
		int* SaveOrUpdateDevelopLogResult
	)
{
    if ( SaveOrUpdateDevelopLogResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateDevelopLog_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.developLog = developLog;

	__atlsoap_hr = SetClientStruct(&__params, 124);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateDevelopLog\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateDevelopLogResult = __params.SaveOrUpdateDevelopLogResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryClientArr(
		int sessionId, 
		bool isProduct, 
		bool isDownloadItem, 
		Client** QueryClientArrResult, int* __QueryClientArrResult_nSizeIs
	)
{
    if ( QueryClientArrResult == NULL )
		return E_POINTER;
    if( __QueryClientArrResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryClientArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.isProduct = isProduct;
	__params.isDownloadItem = isDownloadItem;

	__atlsoap_hr = SetClientStruct(&__params, 125);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryClientArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryClientArrResult = __params.QueryClientArrResult;
	*__QueryClientArrResult_nSizeIs = __params.__QueryClientArrResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateWorkLog(
		int sessionId, 
		WorkLog workLog, 
		int* SaveOrUpdateWorkLogResult
	)
{
    if ( SaveOrUpdateWorkLogResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateWorkLog_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.workLog = workLog;

	__atlsoap_hr = SetClientStruct(&__params, 126);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateWorkLog\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateWorkLogResult = __params.SaveOrUpdateWorkLogResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryCreateTaskUsers(
		int sessionId, 
		User** QueryCreateTaskUsersResult, int* __QueryCreateTaskUsersResult_nSizeIs
	)
{
    if ( QueryCreateTaskUsersResult == NULL )
		return E_POINTER;
    if( __QueryCreateTaskUsersResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryCreateTaskUsers_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 127);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryCreateTaskUsers\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryCreateTaskUsersResult = __params.QueryCreateTaskUsersResult;
	*__QueryCreateTaskUsersResult_nSizeIs = __params.__QueryCreateTaskUsersResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteClient_ProductionByClient(
		int clientId, 
		bool* DeleteClient_ProductionByClientResult
	)
{
    if ( DeleteClient_ProductionByClientResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteClient_ProductionByClient_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.clientId = clientId;

	__atlsoap_hr = SetClientStruct(&__params, 128);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteClient_ProductionByClient\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteClient_ProductionByClientResult = __params.DeleteClient_ProductionByClientResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateReply(
		int sessionId, 
		Reply reply, 
		bool isFile, 
		int* SaveOrUpdateReplyResult
	)
{
    if ( SaveOrUpdateReplyResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateReply_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.reply = reply;
	__params.isFile = isFile;

	__atlsoap_hr = SetClientStruct(&__params, 129);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateReply\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateReplyResult = __params.SaveOrUpdateReplyResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteProduction(
		int sessionId, 
		int productId, 
		bool* DeleteProductionResult
	)
{
    if ( DeleteProductionResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteProduction_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.productId = productId;

	__atlsoap_hr = SetClientStruct(&__params, 130);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteProduction\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteProductionResult = __params.DeleteProductionResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::GetProduction(
		Production* GetProductionResult
	)
{
    if ( GetProductionResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_GetProduction_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 131);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetProduction\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*GetProductionResult = __params.GetProductionResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownLoadTechnoiogyRole(
		int sessionId, 
		Role* DownLoadTechnoiogyRoleResult
	)
{
    if ( DownLoadTechnoiogyRoleResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownLoadTechnoiogyRole_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 132);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownLoadTechnoiogyRole\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownLoadTechnoiogyRoleResult = __params.DownLoadTechnoiogyRoleResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryFeedBackArrByVersionRevision(
		int sessionId, 
		int versionRevisionId, 
		FeedBack** QueryFeedBackArrByVersionRevisionResult, int* __QueryFeedBackArrByVersionRevisionResult_nSizeIs
	)
{
    if ( QueryFeedBackArrByVersionRevisionResult == NULL )
		return E_POINTER;
    if( __QueryFeedBackArrByVersionRevisionResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryFeedBackArrByVersionRevision_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.versionRevisionId = versionRevisionId;

	__atlsoap_hr = SetClientStruct(&__params, 133);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryFeedBackArrByVersionRevision\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryFeedBackArrByVersionRevisionResult = __params.QueryFeedBackArrByVersionRevisionResult;
	*__QueryFeedBackArrByVersionRevisionResult_nSizeIs = __params.__QueryFeedBackArrByVersionRevisionResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateAttendance(
		int sessionId, 
		Attendance attendance, 
		int* SaveOrUpdateAttendanceResult
	)
{
    if ( SaveOrUpdateAttendanceResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateAttendance_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.attendance = attendance;

	__atlsoap_hr = SetClientStruct(&__params, 134);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateAttendance\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateAttendanceResult = __params.SaveOrUpdateAttendanceResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteClientDownloadByClientId(
		int sessionId, 
		int id, 
		bool* DeleteClientDownloadByClientIdResult
	)
{
    if ( DeleteClientDownloadByClientIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteClientDownloadByClientId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 135);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteClientDownloadByClientId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteClientDownloadByClientIdResult = __params.DeleteClientDownloadByClientIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdateRole(
		int sessionId, 
		Role role, 
		int* SaveOrUpdateRoleResult
	)
{
    if ( SaveOrUpdateRoleResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdateRole_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.role = role;

	__atlsoap_hr = SetClientStruct(&__params, 136);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdateRole\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdateRoleResult = __params.SaveOrUpdateRoleResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryAllFeedBackArr(
		int sessionId, 
		FeedBack** QueryAllFeedBackArrResult, int* __QueryAllFeedBackArrResult_nSizeIs
	)
{
    if ( QueryAllFeedBackArrResult == NULL )
		return E_POINTER;
    if( __QueryAllFeedBackArrResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryAllFeedBackArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 137);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryAllFeedBackArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryAllFeedBackArrResult = __params.QueryAllFeedBackArrResult;
	*__QueryAllFeedBackArrResult_nSizeIs = __params.__QueryAllFeedBackArrResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteFeedBackGroup(
		int sessionId, 
		int groupId, 
		bool* DeleteFeedBackGroupResult
	)
{
    if ( DeleteFeedBackGroupResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteFeedBackGroup_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.groupId = groupId;

	__atlsoap_hr = SetClientStruct(&__params, 138);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteFeedBackGroup\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteFeedBackGroupResult = __params.DeleteFeedBackGroupResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SaveOrUpdatePartBuild(
		int sessionId, 
		PartBuild pb, 
		int* SaveOrUpdatePartBuildResult
	)
{
    if ( SaveOrUpdatePartBuildResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SaveOrUpdatePartBuild_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.pb = pb;

	__atlsoap_hr = SetClientStruct(&__params, 139);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SaveOrUpdatePartBuild\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SaveOrUpdatePartBuildResult = __params.SaveOrUpdatePartBuildResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteDevelopLopById(
		int sessionId, 
		int id, 
		bool* DeleteDevelopLopByIdResult
	)
{
    if ( DeleteDevelopLopByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteDevelopLopById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 140);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteDevelopLopById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteDevelopLopByIdResult = __params.DeleteDevelopLopByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownloadWorkLogById(
		int sessionId, 
		int id, 
		WorkLog* DownloadWorkLogByIdResult
	)
{
    if ( DownloadWorkLogByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownloadWorkLogById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 141);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownloadWorkLogById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownloadWorkLogByIdResult = __params.DownloadWorkLogByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryAttendanceArrByUserId(
		int sessionId, 
		int userId, 
		BSTR startDate, 
		BSTR endDate, 
		Attendance** QueryAttendanceArrByUserIdResult, int* __QueryAttendanceArrByUserIdResult_nSizeIs
	)
{
    if ( QueryAttendanceArrByUserIdResult == NULL )
		return E_POINTER;
    if( __QueryAttendanceArrByUserIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryAttendanceArrByUserId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.userId = userId;
	__params.startDate = startDate;
	__params.endDate = endDate;

	__atlsoap_hr = SetClientStruct(&__params, 142);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryAttendanceArrByUserId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryAttendanceArrByUserIdResult = __params.QueryAttendanceArrByUserIdResult;
	*__QueryAttendanceArrByUserIdResult_nSizeIs = __params.__QueryAttendanceArrByUserIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteJobById(
		int sessionId, 
		int id, 
		bool* DeleteJobByIdResult
	)
{
    if ( DeleteJobByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteJobById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 143);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteJobById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteJobByIdResult = __params.DeleteJobByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::UpdateTasksSerialNo(
		int sessionId, 
		int* idArr, int __idArr_nSizeIs, 
		int* serialArr, int __serialArr_nSizeIs, 
		bool* UpdateTasksSerialNoResult
	)
{
    if ( idArr == NULL )
		return E_POINTER;
    if ( serialArr == NULL )
		return E_POINTER;
    if ( UpdateTasksSerialNoResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_UpdateTasksSerialNo_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.idArr = idArr;
	__params.__idArr_nSizeIs = __idArr_nSizeIs;
	__params.serialArr = serialArr;
	__params.__serialArr_nSizeIs = __serialArr_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 144);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/UpdateTasksSerialNo\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*UpdateTasksSerialNoResult = __params.UpdateTasksSerialNoResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryAllUser(
		int sessionId, 
		User** QueryAllUserResult, int* __QueryAllUserResult_nSizeIs
	)
{
    if ( QueryAllUserResult == NULL )
		return E_POINTER;
    if( __QueryAllUserResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryAllUser_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 145);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryAllUser\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryAllUserResult = __params.QueryAllUserResult;
	*__QueryAllUserResult_nSizeIs = __params.__QueryAllUserResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::canExecuteFuncOper(
		int sessionId, 
		int funcOperId, 
		bool* canExecuteFuncOperResult
	)
{
    if ( canExecuteFuncOperResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_canExecuteFuncOper_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.funcOperId = funcOperId;

	__atlsoap_hr = SetClientStruct(&__params, 146);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/canExecuteFuncOper\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*canExecuteFuncOperResult = __params.canExecuteFuncOperResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteFeedBack(
		int sessionId, 
		int id, 
		bool* DeleteFeedBackResult
	)
{
    if ( DeleteFeedBackResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteFeedBack_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 147);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteFeedBack\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteFeedBackResult = __params.DeleteFeedBackResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeletePartBuildById(
		int sessionId, 
		int id, 
		bool* DeletePartBuildByIdResult
	)
{
    if ( DeletePartBuildByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeletePartBuildById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 148);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeletePartBuildById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeletePartBuildByIdResult = __params.DeletePartBuildByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryTestItemByVersionId(
		int sessionId, 
		int versionId, 
		TestItem** QueryTestItemByVersionIdResult, int* __QueryTestItemByVersionIdResult_nSizeIs
	)
{
    if ( QueryTestItemByVersionIdResult == NULL )
		return E_POINTER;
    if( __QueryTestItemByVersionIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryTestItemByVersionId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.versionId = versionId;

	__atlsoap_hr = SetClientStruct(&__params, 149);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryTestItemByVersionId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryTestItemByVersionIdResult = __params.QueryTestItemByVersionIdResult;
	*__QueryTestItemByVersionIdResult_nSizeIs = __params.__QueryTestItemByVersionIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryVersionUpdateInfoArr(
		BSTR production, 
		BSTR version, 
		VersionInfo** QueryVersionUpdateInfoArrResult, int* __QueryVersionUpdateInfoArrResult_nSizeIs
	)
{
    if ( QueryVersionUpdateInfoArrResult == NULL )
		return E_POINTER;
    if( __QueryVersionUpdateInfoArrResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryVersionUpdateInfoArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.production = production;
	__params.version = version;

	__atlsoap_hr = SetClientStruct(&__params, 150);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryVersionUpdateInfoArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryVersionUpdateInfoArrResult = __params.QueryVersionUpdateInfoArrResult;
	*__QueryVersionUpdateInfoArrResult_nSizeIs = __params.__QueryVersionUpdateInfoArrResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryCategoryArr(
		int sessionId, 
		Category** QueryCategoryArrResult, int* __QueryCategoryArrResult_nSizeIs
	)
{
    if ( QueryCategoryArrResult == NULL )
		return E_POINTER;
    if( __QueryCategoryArrResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryCategoryArr_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;

	__atlsoap_hr = SetClientStruct(&__params, 151);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryCategoryArr\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryCategoryArrResult = __params.QueryCategoryArrResult;
	*__QueryCategoryArrResult_nSizeIs = __params.__QueryCategoryArrResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryFeedBacks(
		int sessionId, 
		int gradeId, 
		BSTR problemType, 
		BSTR processState, 
		int* productIdArr, int __productIdArr_nSizeIs, 
		int pageNum, 
		int pageSize, 
		FeedBack** QueryFeedBacksResult, int* __QueryFeedBacksResult_nSizeIs
	)
{
    if ( productIdArr == NULL )
		return E_POINTER;
    if ( QueryFeedBacksResult == NULL )
		return E_POINTER;
    if( __QueryFeedBacksResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryFeedBacks_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.gradeId = gradeId;
	__params.problemType = problemType;
	__params.processState = processState;
	__params.productIdArr = productIdArr;
	__params.__productIdArr_nSizeIs = __productIdArr_nSizeIs;
	__params.pageNum = pageNum;
	__params.pageSize = pageSize;

	__atlsoap_hr = SetClientStruct(&__params, 152);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryFeedBacks\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryFeedBacksResult = __params.QueryFeedBacksResult;
	*__QueryFeedBacksResult_nSizeIs = __params.__QueryFeedBacksResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::InserClientDownloadItem(
		int sessionId, 
		Client_DownloadItem cd, 
		bool* InserClientDownloadItemResult
	)
{
    if ( InserClientDownloadItemResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_InserClientDownloadItem_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.cd = cd;

	__atlsoap_hr = SetClientStruct(&__params, 153);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/InserClientDownloadItem\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*InserClientDownloadItemResult = __params.InserClientDownloadItemResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteVersionRevision(
		int sessionId, 
		int id, 
		bool* DeleteVersionRevisionResult
	)
{
    if ( DeleteVersionRevisionResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteVersionRevision_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 154);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteVersionRevision\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteVersionRevisionResult = __params.DeleteVersionRevisionResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::UpdateWorklog_testItem(
		int sessionId, 
		WorLog_testItem* worklogTestItemArr, int __worklogTestItemArr_nSizeIs, 
		WorkLog wl, 
		bool* UpdateWorklog_testItemResult
	)
{
    if ( worklogTestItemArr == NULL )
		return E_POINTER;
    if ( UpdateWorklog_testItemResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_UpdateWorklog_testItem_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.worklogTestItemArr = worklogTestItemArr;
	__params.__worklogTestItemArr_nSizeIs = __worklogTestItemArr_nSizeIs;
	__params.wl = wl;

	__atlsoap_hr = SetClientStruct(&__params, 155);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/UpdateWorklog_testItem\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*UpdateWorklog_testItemResult = __params.UpdateWorklog_testItemResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryTaskDispatchArrByReceiverId(
		int sessionId, 
		int userId, 
		bool finished, 
		bool unFinished, 
		BSTR startDate, 
		BSTR endDate, 
		TaskDispatch** QueryTaskDispatchArrByReceiverIdResult, int* __QueryTaskDispatchArrByReceiverIdResult_nSizeIs
	)
{
    if ( QueryTaskDispatchArrByReceiverIdResult == NULL )
		return E_POINTER;
    if( __QueryTaskDispatchArrByReceiverIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryTaskDispatchArrByReceiverId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.userId = userId;
	__params.finished = finished;
	__params.unFinished = unFinished;
	__params.startDate = startDate;
	__params.endDate = endDate;

	__atlsoap_hr = SetClientStruct(&__params, 156);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryTaskDispatchArrByReceiverId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryTaskDispatchArrByReceiverIdResult = __params.QueryTaskDispatchArrByReceiverIdResult;
	*__QueryTaskDispatchArrByReceiverIdResult_nSizeIs = __params.__QueryTaskDispatchArrByReceiverIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DeleteNewsById(
		int sessionId, 
		int id, 
		bool* DeleteNewsByIdResult
	)
{
    if ( DeleteNewsByIdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DeleteNewsById_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 157);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DeleteNewsById\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DeleteNewsByIdResult = __params.DeleteNewsByIdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownloadUser(
		int sessionId, 
		int id, 
		User* DownloadUserResult
	)
{
    if ( DownloadUserResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownloadUser_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 158);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownloadUser\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownloadUserResult = __params.DownloadUserResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::LoginUser(
		BSTR userName, 
		BSTR password, 
		LoginClass* LoginUserResult
	)
{
    if ( LoginUserResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_LoginUser_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.userName = userName;
	__params.password = password;

	__atlsoap_hr = SetClientStruct(&__params, 159);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/LoginUser\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*LoginUserResult = __params.LoginUserResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryAllPartBuild(
		PartBuild** QueryAllPartBuildResult, int* __QueryAllPartBuildResult_nSizeIs
	)
{
    if ( QueryAllPartBuildResult == NULL )
		return E_POINTER;
    if( __QueryAllPartBuildResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryAllPartBuild_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 160);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryAllPartBuild\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryAllPartBuildResult = __params.QueryAllPartBuildResult;
	*__QueryAllPartBuildResult_nSizeIs = __params.__QueryAllPartBuildResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryVersionTestItemArrByVersionId(
		int sessionId, 
		int versionId, 
		Version_testItem** QueryVersionTestItemArrByVersionIdResult, int* __QueryVersionTestItemArrByVersionIdResult_nSizeIs
	)
{
    if ( QueryVersionTestItemArrByVersionIdResult == NULL )
		return E_POINTER;
    if( __QueryVersionTestItemArrByVersionIdResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryVersionTestItemArrByVersionId_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.versionId = versionId;

	__atlsoap_hr = SetClientStruct(&__params, 161);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryVersionTestItemArrByVersionId\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryVersionTestItemArrByVersionIdResult = __params.QueryVersionTestItemArrByVersionIdResult;
	*__QueryVersionTestItemArrByVersionIdResult_nSizeIs = __params.__QueryVersionTestItemArrByVersionIdResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::GetFeedBack(
		FeedBack* GetFeedBackResult
	)
{
    if ( GetFeedBackResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_GetFeedBack_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 162);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetFeedBack\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*GetFeedBackResult = __params.GetFeedBackResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::UpdateVersion_TestItem(
		int sessionId, 
		Version_testItem vt, 
		bool* UpdateVersion_TestItemResult
	)
{
    if ( UpdateVersion_TestItemResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_UpdateVersion_TestItem_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.vt = vt;

	__atlsoap_hr = SetClientStruct(&__params, 163);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/UpdateVersion_TestItem\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*UpdateVersion_TestItemResult = __params.UpdateVersion_TestItemResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::DownloadProblemGrade(
		int sessionId, 
		int id, 
		ProblemGrade* DownloadProblemGradeResult
	)
{
    if ( DownloadProblemGradeResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_DownloadProblemGrade_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.id = id;

	__atlsoap_hr = SetClientStruct(&__params, 164);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/DownloadProblemGrade\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*DownloadProblemGradeResult = __params.DownloadProblemGradeResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::SetSqlLink(
		BSTR userName, 
		BSTR password, 
		BSTR linkStr, 
		bool* SetSqlLinkResult
	)
{
    if ( SetSqlLinkResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_SetSqlLink_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.userName = userName;
	__params.password = password;
	__params.linkStr = linkStr;

	__atlsoap_hr = SetClientStruct(&__params, 165);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SetSqlLink\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*SetSqlLinkResult = __params.SetSqlLinkResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CXMServerT<TClient>::QueryNewsArrByDate(
		int sessionId, 
		BSTR startDate, 
		BSTR endDate, 
		News** QueryNewsArrByDateResult, int* __QueryNewsArrByDateResult_nSizeIs
	)
{
    if ( QueryNewsArrByDateResult == NULL )
		return E_POINTER;
    if( __QueryNewsArrByDateResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CXMServer_QueryNewsArrByDate_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sessionId = sessionId;
	__params.startDate = startDate;
	__params.endDate = endDate;

	__atlsoap_hr = SetClientStruct(&__params, 166);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/QueryNewsArrByDate\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*QueryNewsArrByDateResult = __params.QueryNewsArrByDateResult;
	*__QueryNewsArrByDateResult_nSizeIs = __params.__QueryNewsArrByDateResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CXMServerT<TClient>::GetFunctionMap()
{
	return __CXMServer_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CXMServerT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CXMServer_GetUser_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_GetUser_atlsoapheader_map = 
	{
		0x617A5F8E,
		"GetUser",
		L"GetUserResponse",
		sizeof("GetUser")-1,
		sizeof("GetUserResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_GetUser_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownloadNotification_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownloadNotification_atlsoapheader_map = 
	{
		0xD68C9D4E,
		"DownloadNotification",
		L"DownloadNotificationResponse",
		sizeof("DownloadNotification")-1,
		sizeof("DownloadNotificationResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownloadNotification_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateTaskDispatch_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateTaskDispatch_atlsoapheader_map = 
	{
		0x247E2C45,
		"SaveOrUpdateTaskDispatch",
		L"SaveOrUpdateTaskDispatchResponse",
		sizeof("SaveOrUpdateTaskDispatch")-1,
		sizeof("SaveOrUpdateTaskDispatchResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateTaskDispatch_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateDownloadItem_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateDownloadItem_atlsoapheader_map = 
	{
		0x0F97A0C9,
		"SaveOrUpdateDownloadItem",
		L"SaveOrUpdateDownloadItemResponse",
		sizeof("SaveOrUpdateDownloadItem")-1,
		sizeof("SaveOrUpdateDownloadItemResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateDownloadItem_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryFeedBackArrByProblemType_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryFeedBackArrByProblemType_atlsoapheader_map = 
	{
		0x6BC9FE3D,
		"QueryFeedBackArrByProblemType",
		L"QueryFeedBackArrByProblemTypeResponse",
		sizeof("QueryFeedBackArrByProblemType")-1,
		sizeof("QueryFeedBackArrByProblemTypeResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryFeedBackArrByProblemType_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryProblemGradeArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryProblemGradeArr_atlsoapheader_map = 
	{
		0xC63AECFE,
		"QueryProblemGradeArr",
		L"QueryProblemGradeArrResponse",
		sizeof("QueryProblemGradeArr")-1,
		sizeof("QueryProblemGradeArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryProblemGradeArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_UpdateUser_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_UpdateUser_atlsoapheader_map = 
	{
		0xDA5C5851,
		"UpdateUser",
		L"UpdateUserResponse",
		sizeof("UpdateUser")-1,
		sizeof("UpdateUserResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_UpdateUser_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownLoadNews_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownLoadNews_atlsoapheader_map = 
	{
		0x6BFE15C4,
		"DownLoadNews",
		L"DownLoadNewsResponse",
		sizeof("DownLoadNews")-1,
		sizeof("DownLoadNewsResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownLoadNews_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryProductionArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryProductionArr_atlsoapheader_map = 
	{
		0xB6DF7EB1,
		"QueryProductionArr",
		L"QueryProductionArrResponse",
		sizeof("QueryProductionArr")-1,
		sizeof("QueryProductionArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryProductionArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryAllRole_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryAllRole_atlsoapheader_map = 
	{
		0xC059CD50,
		"QueryAllRole",
		L"QueryAllRoleResponse",
		sizeof("QueryAllRole")-1,
		sizeof("QueryAllRoleResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryAllRole_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_InsertUser_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_InsertUser_atlsoapheader_map = 
	{
		0xFFEB0543,
		"InsertUser",
		L"InsertUserResponse",
		sizeof("InsertUser")-1,
		sizeof("InsertUserResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_InsertUser_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_UpdateUserTask_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_UpdateUserTask_atlsoapheader_map = 
	{
		0x754C5804,
		"UpdateUserTask",
		L"UpdateUserTaskResponse",
		sizeof("UpdateUserTask")-1,
		sizeof("UpdateUserTaskResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_UpdateUserTask_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryChildTaskDispatchsByGroupId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryChildTaskDispatchsByGroupId_atlsoapheader_map = 
	{
		0x18E35034,
		"QueryChildTaskDispatchsByGroupId",
		L"QueryChildTaskDispatchsByGroupIdResponse",
		sizeof("QueryChildTaskDispatchsByGroupId")-1,
		sizeof("QueryChildTaskDispatchsByGroupIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryChildTaskDispatchsByGroupId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_UpdateTestItemArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_UpdateTestItemArr_atlsoapheader_map = 
	{
		0x479D25A6,
		"UpdateTestItemArr",
		L"UpdateTestItemArrResponse",
		sizeof("UpdateTestItemArr")-1,
		sizeof("UpdateTestItemArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_UpdateTestItemArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_GetVersionRevision_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_GetVersionRevision_atlsoapheader_map = 
	{
		0xB697DBC4,
		"GetVersionRevision",
		L"GetVersionRevisionResponse",
		sizeof("GetVersionRevision")-1,
		sizeof("GetVersionRevisionResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_GetVersionRevision_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteCategoryById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteCategoryById_atlsoapheader_map = 
	{
		0xF0F06A08,
		"DeleteCategoryById",
		L"DeleteCategoryByIdResponse",
		sizeof("DeleteCategoryById")-1,
		sizeof("DeleteCategoryByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteCategoryById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryVersionArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryVersionArr_atlsoapheader_map = 
	{
		0x59AAB150,
		"QueryVersionArr",
		L"QueryVersionArrResponse",
		sizeof("QueryVersionArr")-1,
		sizeof("QueryVersionArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryVersionArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryTestItemSetArrByProId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryTestItemSetArrByProId_atlsoapheader_map = 
	{
		0x832E857E,
		"QueryTestItemSetArrByProId",
		L"QueryTestItemSetArrByProIdResponse",
		sizeof("QueryTestItemSetArrByProId")-1,
		sizeof("QueryTestItemSetArrByProIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryTestItemSetArrByProId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_GetFileHeader_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_GetFileHeader_atlsoapheader_map = 
	{
		0x46D705F8,
		"GetFileHeader",
		L"GetFileHeaderResponse",
		sizeof("GetFileHeader")-1,
		sizeof("GetFileHeaderResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_GetFileHeader_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_GetReply_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_GetReply_atlsoapheader_map = 
	{
		0x9F8BA01B,
		"GetReply",
		L"GetReplyResponse",
		sizeof("GetReply")-1,
		sizeof("GetReplyResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_GetReply_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_ValidateUserPerm_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_ValidateUserPerm_atlsoapheader_map = 
	{
		0x20BEFA0C,
		"ValidateUserPerm",
		L"ValidateUserPermResponse",
		sizeof("ValidateUserPerm")-1,
		sizeof("ValidateUserPermResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_ValidateUserPerm_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_UpdateTaskDispatchArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_UpdateTaskDispatchArr_atlsoapheader_map = 
	{
		0x1714191A,
		"UpdateTaskDispatchArr",
		L"UpdateTaskDispatchArrResponse",
		sizeof("UpdateTaskDispatchArr")-1,
		sizeof("UpdateTaskDispatchArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_UpdateTaskDispatchArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateProduction_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateProduction_atlsoapheader_map = 
	{
		0x87361049,
		"SaveOrUpdateProduction",
		L"SaveOrUpdateProductionResponse",
		sizeof("SaveOrUpdateProduction")-1,
		sizeof("SaveOrUpdateProductionResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateProduction_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_InsertFeedBackArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_InsertFeedBackArr_atlsoapheader_map = 
	{
		0x1E2227AE,
		"InsertFeedBackArr",
		L"InsertFeedBackArrResponse",
		sizeof("InsertFeedBackArr")-1,
		sizeof("InsertFeedBackArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_InsertFeedBackArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownloadProduction_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownloadProduction_atlsoapheader_map = 
	{
		0xC3604D6E,
		"DownloadProduction",
		L"DownloadProductionResponse",
		sizeof("DownloadProduction")-1,
		sizeof("DownloadProductionResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownloadProduction_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteWorkLog_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteWorkLog_atlsoapheader_map = 
	{
		0x4C096DC7,
		"DeleteWorkLog",
		L"DeleteWorkLogResponse",
		sizeof("DeleteWorkLog")-1,
		sizeof("DeleteWorkLogResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteWorkLog_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryTestItemArrBySetId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryTestItemArrBySetId_atlsoapheader_map = 
	{
		0xC577E6ED,
		"QueryTestItemArrBySetId",
		L"QueryTestItemArrBySetIdResponse",
		sizeof("QueryTestItemArrBySetId")-1,
		sizeof("QueryTestItemArrBySetIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryTestItemArrBySetId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryClientProductByClientId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryClientProductByClientId_atlsoapheader_map = 
	{
		0xB7A3230C,
		"QueryClientProductByClientId",
		L"QueryClientProductByClientIdResponse",
		sizeof("QueryClientProductByClientId")-1,
		sizeof("QueryClientProductByClientIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryClientProductByClientId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryProductionByUserId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryProductionByUserId_atlsoapheader_map = 
	{
		0x60DF98D3,
		"QueryProductionByUserId",
		L"QueryProductionByUserIdResponse",
		sizeof("QueryProductionByUserId")-1,
		sizeof("QueryProductionByUserIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryProductionByUserId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryTestItemArrByCondition_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryTestItemArrByCondition_atlsoapheader_map = 
	{
		0x668C44FB,
		"QueryTestItemArrByCondition",
		L"QueryTestItemArrByConditionResponse",
		sizeof("QueryTestItemArrByCondition")-1,
		sizeof("QueryTestItemArrByConditionResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryTestItemArrByCondition_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryproductionsByClientId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryproductionsByClientId_atlsoapheader_map = 
	{
		0x94F67386,
		"QueryproductionsByClientId",
		L"QueryproductionsByClientIdResponse",
		sizeof("QueryproductionsByClientId")-1,
		sizeof("QueryproductionsByClientIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryproductionsByClientId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownloadFeedBack_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownloadFeedBack_atlsoapheader_map = 
	{
		0xF406F54C,
		"DownloadFeedBack",
		L"DownloadFeedBackResponse",
		sizeof("DownloadFeedBack")-1,
		sizeof("DownloadFeedBackResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownloadFeedBack_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_GetWorkLog_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_GetWorkLog_atlsoapheader_map = 
	{
		0xD2BF6E14,
		"GetWorkLog",
		L"GetWorkLogResponse",
		sizeof("GetWorkLog")-1,
		sizeof("GetWorkLogResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_GetWorkLog_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteClientById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteClientById_atlsoapheader_map = 
	{
		0x22A93929,
		"DeleteClientById",
		L"DeleteClientByIdResponse",
		sizeof("DeleteClientById")-1,
		sizeof("DeleteClientByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteClientById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteTaskDispatch_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteTaskDispatch_atlsoapheader_map = 
	{
		0x82351205,
		"DeleteTaskDispatch",
		L"DeleteTaskDispatchResponse",
		sizeof("DeleteTaskDispatch")-1,
		sizeof("DeleteTaskDispatchResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteTaskDispatch_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_GetRole_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_GetRole_atlsoapheader_map = 
	{
		0xEB25AD41,
		"GetRole",
		L"GetRoleResponse",
		sizeof("GetRole")-1,
		sizeof("GetRoleResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_GetRole_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryDownloadItemByClient_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryDownloadItemByClient_atlsoapheader_map = 
	{
		0xD545A946,
		"QueryDownloadItemByClient",
		L"QueryDownloadItemByClientResponse",
		sizeof("QueryDownloadItemByClient")-1,
		sizeof("QueryDownloadItemByClientResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryDownloadItemByClient_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteDownloadItem_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteDownloadItem_atlsoapheader_map = 
	{
		0x6D4E8689,
		"DeleteDownloadItem",
		L"DeleteDownloadItemResponse",
		sizeof("DeleteDownloadItem")-1,
		sizeof("DeleteDownloadItemResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteDownloadItem_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteClient_DownloadItemByItem_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteClient_DownloadItemByItem_atlsoapheader_map = 
	{
		0x69FA4811,
		"DeleteClient_DownloadItemByItem",
		L"DeleteClient_DownloadItemByItemResponse",
		sizeof("DeleteClient_DownloadItemByItem")-1,
		sizeof("DeleteClient_DownloadItemByItemResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteClient_DownloadItemByItem_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryEssentialFeedBacks_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryEssentialFeedBacks_atlsoapheader_map = 
	{
		0x3A645FA5,
		"QueryEssentialFeedBacks",
		L"QueryEssentialFeedBacksResponse",
		sizeof("QueryEssentialFeedBacks")-1,
		sizeof("QueryEssentialFeedBacksResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryEssentialFeedBacks_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryFeedBackArrByProcessState_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryFeedBackArrByProcessState_atlsoapheader_map = 
	{
		0x89A54E4A,
		"QueryFeedBackArrByProcessState",
		L"QueryFeedBackArrByProcessStateResponse",
		sizeof("QueryFeedBackArrByProcessState")-1,
		sizeof("QueryFeedBackArrByProcessStateResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryFeedBackArrByProcessState_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryCategoryArrByObjType_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryCategoryArrByObjType_atlsoapheader_map = 
	{
		0x8A202140,
		"QueryCategoryArrByObjType",
		L"QueryCategoryArrByObjTypeResponse",
		sizeof("QueryCategoryArrByObjType")-1,
		sizeof("QueryCategoryArrByObjTypeResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryCategoryArrByObjType_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateJob_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateJob_atlsoapheader_map = 
	{
		0x7E85329D,
		"SaveOrUpdateJob",
		L"SaveOrUpdateJobResponse",
		sizeof("SaveOrUpdateJob")-1,
		sizeof("SaveOrUpdateJobResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateJob_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryFeedBacksToDataSet_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryFeedBacksToDataSet_atlsoapheader_map = 
	{
		0x1AE66B06,
		"QueryFeedBacksToDataSet",
		L"QueryFeedBacksToDataSetResponse",
		sizeof("QueryFeedBacksToDataSet")-1,
		sizeof("QueryFeedBacksToDataSetResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryFeedBacksToDataSet_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateNotification_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateNotification_atlsoapheader_map = 
	{
		0xE6DE82E9,
		"SaveOrUpdateNotification",
		L"SaveOrUpdateNotificationResponse",
		sizeof("SaveOrUpdateNotification")-1,
		sizeof("SaveOrUpdateNotificationResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateNotification_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_InsertUser_Role_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_InsertUser_Role_atlsoapheader_map = 
	{
		0xC018E834,
		"InsertUser_Role",
		L"InsertUser_RoleResponse",
		sizeof("InsertUser_Role")-1,
		sizeof("InsertUser_RoleResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_InsertUser_Role_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateVersionRevision_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateVersionRevision_atlsoapheader_map = 
	{
		0x4881D0B7,
		"SaveOrUpdateVersionRevision",
		L"SaveOrUpdateVersionRevisionResponse",
		sizeof("SaveOrUpdateVersionRevision")-1,
		sizeof("SaveOrUpdateVersionRevisionResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateVersionRevision_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryTestItemByWorkLogDate_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryTestItemByWorkLogDate_atlsoapheader_map = 
	{
		0xBB720A52,
		"QueryTestItemByWorkLogDate",
		L"QueryTestItemByWorkLogDateResponse",
		sizeof("QueryTestItemByWorkLogDate")-1,
		sizeof("QueryTestItemByWorkLogDateResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryTestItemByWorkLogDate_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryFeedBackArrByName_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryFeedBackArrByName_atlsoapheader_map = 
	{
		0x09A69D6B,
		"QueryFeedBackArrByName",
		L"QueryFeedBackArrByNameResponse",
		sizeof("QueryFeedBackArrByName")-1,
		sizeof("QueryFeedBackArrByNameResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryFeedBackArrByName_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteUser_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteUser_atlsoapheader_map = 
	{
		0xAA4E4421,
		"DeleteUser",
		L"DeleteUserResponse",
		sizeof("DeleteUser")-1,
		sizeof("DeleteUserResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteUser_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateCategory_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateCategory_atlsoapheader_map = 
	{
		0x32589E40,
		"SaveOrUpdateCategory",
		L"SaveOrUpdateCategoryResponse",
		sizeof("SaveOrUpdateCategory")-1,
		sizeof("SaveOrUpdateCategoryResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateCategory_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownloadClientProductById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownloadClientProductById_atlsoapheader_map = 
	{
		0x279A7AEF,
		"DownloadClientProductById",
		L"DownloadClientProductByIdResponse",
		sizeof("DownloadClientProductById")-1,
		sizeof("DownloadClientProductByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownloadClientProductById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryLatestVersionRevisionArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryLatestVersionRevisionArr_atlsoapheader_map = 
	{
		0xAD830A8C,
		"QueryLatestVersionRevisionArr",
		L"QueryLatestVersionRevisionArrResponse",
		sizeof("QueryLatestVersionRevisionArr")-1,
		sizeof("QueryLatestVersionRevisionArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryLatestVersionRevisionArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteReply_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteReply_atlsoapheader_map = 
	{
		0x02DC170E,
		"DeleteReply",
		L"DeleteReplyResponse",
		sizeof("DeleteReply")-1,
		sizeof("DeleteReplyResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteReply_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryTaskDispatchs_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryTaskDispatchs_atlsoapheader_map = 
	{
		0xAC67267B,
		"QueryTaskDispatchs",
		L"QueryTaskDispatchsResponse",
		sizeof("QueryTaskDispatchs")-1,
		sizeof("QueryTaskDispatchsResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryTaskDispatchs_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownloadAttendanceById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownloadAttendanceById_atlsoapheader_map = 
	{
		0x10B93D46,
		"DownloadAttendanceById",
		L"DownloadAttendanceByIdResponse",
		sizeof("DownloadAttendanceById")-1,
		sizeof("DownloadAttendanceByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownloadAttendanceById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownloadCategoryById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownloadCategoryById_atlsoapheader_map = 
	{
		0x82E79EAD,
		"DownloadCategoryById",
		L"DownloadCategoryByIdResponse",
		sizeof("DownloadCategoryById")-1,
		sizeof("DownloadCategoryByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownloadCategoryById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryDevelopLogArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryDevelopLogArr_atlsoapheader_map = 
	{
		0x1F1E5C3B,
		"QueryDevelopLogArr",
		L"QueryDevelopLogArrResponse",
		sizeof("QueryDevelopLogArr")-1,
		sizeof("QueryDevelopLogArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryDevelopLogArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownloadItemById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownloadItemById_atlsoapheader_map = 
	{
		0x5C64547E,
		"DownloadItemById",
		L"DownloadItemByIdResponse",
		sizeof("DownloadItemById")-1,
		sizeof("DownloadItemByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownloadItemById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_InsertTaskDispatchArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_InsertTaskDispatchArr_atlsoapheader_map = 
	{
		0xB0AD8ACC,
		"InsertTaskDispatchArr",
		L"InsertTaskDispatchArrResponse",
		sizeof("InsertTaskDispatchArr")-1,
		sizeof("InsertTaskDispatchArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_InsertTaskDispatchArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownLoadLasrVersionByProId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownLoadLasrVersionByProId_atlsoapheader_map = 
	{
		0x12C80058,
		"DownLoadLasrVersionByProId",
		L"DownLoadLasrVersionByProIdResponse",
		sizeof("DownLoadLasrVersionByProId")-1,
		sizeof("DownLoadLasrVersionByProIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownLoadLasrVersionByProId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryClientByProvinceNum_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryClientByProvinceNum_atlsoapheader_map = 
	{
		0x26078375,
		"QueryClientByProvinceNum",
		L"QueryClientByProvinceNumResponse",
		sizeof("QueryClientByProvinceNum")-1,
		sizeof("QueryClientByProvinceNumResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryClientByProvinceNum_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteWorkLogById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteWorkLogById_atlsoapheader_map = 
	{
		0xB044BF4F,
		"DeleteWorkLogById",
		L"DeleteWorkLogByIdResponse",
		sizeof("DeleteWorkLogById")-1,
		sizeof("DeleteWorkLogByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteWorkLogById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryFeedBackArrByProposerId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryFeedBackArrByProposerId_atlsoapheader_map = 
	{
		0x6F83BA91,
		"QueryFeedBackArrByProposerId",
		L"QueryFeedBackArrByProposerIdResponse",
		sizeof("QueryFeedBackArrByProposerId")-1,
		sizeof("QueryFeedBackArrByProposerIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryFeedBackArrByProposerId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownLoadJobById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownLoadJobById_atlsoapheader_map = 
	{
		0x27DBFE4A,
		"DownLoadJobById",
		L"DownLoadJobByIdResponse",
		sizeof("DownLoadJobById")-1,
		sizeof("DownLoadJobByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownLoadJobById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateTestItem_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateTestItem_atlsoapheader_map = 
	{
		0x2E3B1C51,
		"SaveOrUpdateTestItem",
		L"SaveOrUpdateTestItemResponse",
		sizeof("SaveOrUpdateTestItem")-1,
		sizeof("SaveOrUpdateTestItemResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateTestItem_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryAllJobs_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryAllJobs_atlsoapheader_map = 
	{
		0x6F23650C,
		"QueryAllJobs",
		L"QueryAllJobsResponse",
		sizeof("QueryAllJobs")-1,
		sizeof("QueryAllJobsResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryAllJobs_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryPartBuildByPartItemId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryPartBuildByPartItemId_atlsoapheader_map = 
	{
		0x5F093C9A,
		"QueryPartBuildByPartItemId",
		L"QueryPartBuildByPartItemIdResponse",
		sizeof("QueryPartBuildByPartItemId")-1,
		sizeof("QueryPartBuildByPartItemIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryPartBuildByPartItemId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryRolesByUserId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryRolesByUserId_atlsoapheader_map = 
	{
		0xAE0AF8D1,
		"QueryRolesByUserId",
		L"QueryRolesByUserIdResponse",
		sizeof("QueryRolesByUserId")-1,
		sizeof("QueryRolesByUserIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryRolesByUserId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryFeedBackArrByProblemGrade_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryFeedBackArrByProblemGrade_atlsoapheader_map = 
	{
		0x9FD1DC3E,
		"QueryFeedBackArrByProblemGrade",
		L"QueryFeedBackArrByProblemGradeResponse",
		sizeof("QueryFeedBackArrByProblemGrade")-1,
		sizeof("QueryFeedBackArrByProblemGradeResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryFeedBackArrByProblemGrade_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteTestItemSet_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteTestItemSet_atlsoapheader_map = 
	{
		0x1749E95D,
		"DeleteTestItemSet",
		L"DeleteTestItemSetResponse",
		sizeof("DeleteTestItemSet")-1,
		sizeof("DeleteTestItemSetResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteTestItemSet_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryFeedBackArrByCondition_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryFeedBackArrByCondition_atlsoapheader_map = 
	{
		0x66109C91,
		"QueryFeedBackArrByCondition",
		L"QueryFeedBackArrByConditionResponse",
		sizeof("QueryFeedBackArrByCondition")-1,
		sizeof("QueryFeedBackArrByConditionResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryFeedBackArrByCondition_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownloadVersionRevision_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownloadVersionRevision_atlsoapheader_map = 
	{
		0x4FE90CFC,
		"DownloadVersionRevision",
		L"DownloadVersionRevisionResponse",
		sizeof("DownloadVersionRevision")-1,
		sizeof("DownloadVersionRevisionResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownloadVersionRevision_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteAttendance_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteAttendance_atlsoapheader_map = 
	{
		0x7DAFC0D9,
		"DeleteAttendance",
		L"DeleteAttendanceResponse",
		sizeof("DeleteAttendance")-1,
		sizeof("DeleteAttendanceResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteAttendance_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_ValidateUserName_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_ValidateUserName_atlsoapheader_map = 
	{
		0xF14D0C99,
		"ValidateUserName",
		L"ValidateUserNameResponse",
		sizeof("ValidateUserName")-1,
		sizeof("ValidateUserNameResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_ValidateUserName_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteTestItemById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteTestItemById_atlsoapheader_map = 
	{
		0xF7158899,
		"DeleteTestItemById",
		L"DeleteTestItemByIdResponse",
		sizeof("DeleteTestItemById")-1,
		sizeof("DeleteTestItemByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteTestItemById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownloadTaskDispatch_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownloadTaskDispatch_atlsoapheader_map = 
	{
		0x142C46AA,
		"DownloadTaskDispatch",
		L"DownloadTaskDispatchResponse",
		sizeof("DownloadTaskDispatch")-1,
		sizeof("DownloadTaskDispatchResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownloadTaskDispatch_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryFeedBacksCount_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryFeedBacksCount_atlsoapheader_map = 
	{
		0x88868846,
		"QueryFeedBacksCount",
		L"QueryFeedBacksCountResponse",
		sizeof("QueryFeedBacksCount")-1,
		sizeof("QueryFeedBacksCountResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryFeedBacksCount_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryInsideUsers_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryInsideUsers_atlsoapheader_map = 
	{
		0xE0A577F3,
		"QueryInsideUsers",
		L"QueryInsideUsersResponse",
		sizeof("QueryInsideUsers")-1,
		sizeof("QueryInsideUsersResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryInsideUsers_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_GetLoginStruct_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_GetLoginStruct_atlsoapheader_map = 
	{
		0xAB94F3AD,
		"GetLoginStruct",
		L"GetLoginStructResponse",
		sizeof("GetLoginStruct")-1,
		sizeof("GetLoginStructResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_GetLoginStruct_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryTestItemArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryTestItemArr_atlsoapheader_map = 
	{
		0x7C7A90F9,
		"QueryTestItemArr",
		L"QueryTestItemArrResponse",
		sizeof("QueryTestItemArr")-1,
		sizeof("QueryTestItemArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryTestItemArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteNotification_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteNotification_atlsoapheader_map = 
	{
		0x449568A9,
		"DeleteNotification",
		L"DeleteNotificationResponse",
		sizeof("DeleteNotification")-1,
		sizeof("DeleteNotificationResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteNotification_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateNews_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateNews_atlsoapheader_map = 
	{
		0x7EC24C7F,
		"SaveOrUpdateNews",
		L"SaveOrUpdateNewsResponse",
		sizeof("SaveOrUpdateNews")-1,
		sizeof("SaveOrUpdateNewsResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateNews_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryVersionRevisionArrByProductionId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryVersionRevisionArrByProductionId_atlsoapheader_map = 
	{
		0x10C0D0CE,
		"QueryVersionRevisionArrByProductionId",
		L"QueryVersionRevisionArrByProductionIdResponse",
		sizeof("QueryVersionRevisionArrByProductionId")-1,
		sizeof("QueryVersionRevisionArrByProductionIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryVersionRevisionArrByProductionId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryFeedBackArrByProductionId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryFeedBackArrByProductionId_atlsoapheader_map = 
	{
		0x67774CDE,
		"QueryFeedBackArrByProductionId",
		L"QueryFeedBackArrByProductionIdResponse",
		sizeof("QueryFeedBackArrByProductionId")-1,
		sizeof("QueryFeedBackArrByProductionIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryFeedBackArrByProductionId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteDeveLop_testItemByDevelopLopId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteDeveLop_testItemByDevelopLopId_atlsoapheader_map = 
	{
		0xA5C2A421,
		"DeleteDeveLop_testItemByDevelopLopId",
		L"DeleteDeveLop_testItemByDevelopLopIdResponse",
		sizeof("DeleteDeveLop_testItemByDevelopLopId")-1,
		sizeof("DeleteDeveLop_testItemByDevelopLopIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteDeveLop_testItemByDevelopLopId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteClient_DownloadItemByClient_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteClient_DownloadItemByClient_atlsoapheader_map = 
	{
		0x16C6FEE1,
		"DeleteClient_DownloadItemByClient",
		L"DeleteClient_DownloadItemByClientResponse",
		sizeof("DeleteClient_DownloadItemByClient")-1,
		sizeof("DeleteClient_DownloadItemByClientResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteClient_DownloadItemByClient_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryAttendanceByDate_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryAttendanceByDate_atlsoapheader_map = 
	{
		0x24DF18F5,
		"QueryAttendanceByDate",
		L"QueryAttendanceByDateResponse",
		sizeof("QueryAttendanceByDate")-1,
		sizeof("QueryAttendanceByDateResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryAttendanceByDate_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_GetTaskDispatch_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_GetTaskDispatch_atlsoapheader_map = 
	{
		0x5E484A72,
		"GetTaskDispatch",
		L"GetTaskDispatchResponse",
		sizeof("GetTaskDispatch")-1,
		sizeof("GetTaskDispatchResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_GetTaskDispatch_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteRole_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteRole_atlsoapheader_map = 
	{
		0x33F991D4,
		"DeleteRole",
		L"DeleteRoleResponse",
		sizeof("DeleteRole")-1,
		sizeof("DeleteRoleResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteRole_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_UpdateVersion_TestItemArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_UpdateVersion_TestItemArr_atlsoapheader_map = 
	{
		0xA2E3846B,
		"UpdateVersion_TestItemArr",
		L"UpdateVersion_TestItemArrResponse",
		sizeof("UpdateVersion_TestItemArr")-1,
		sizeof("UpdateVersion_TestItemArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_UpdateVersion_TestItemArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateClient_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateClient_atlsoapheader_map = 
	{
		0x6AA974E1,
		"SaveOrUpdateClient",
		L"SaveOrUpdateClientResponse",
		sizeof("SaveOrUpdateClient")-1,
		sizeof("SaveOrUpdateClientResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateClient_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryAllDevelopLog_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryAllDevelopLog_atlsoapheader_map = 
	{
		0x432D284F,
		"QueryAllDevelopLog",
		L"QueryAllDevelopLogResponse",
		sizeof("QueryAllDevelopLog")-1,
		sizeof("QueryAllDevelopLogResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryAllDevelopLog_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_UpdateFeedBacksSerialNo_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_UpdateFeedBacksSerialNo_atlsoapheader_map = 
	{
		0x52BC0027,
		"UpdateFeedBacksSerialNo",
		L"UpdateFeedBacksSerialNoResponse",
		sizeof("UpdateFeedBacksSerialNo")-1,
		sizeof("UpdateFeedBacksSerialNoResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_UpdateFeedBacksSerialNo_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownloadFileHeader_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownloadFileHeader_atlsoapheader_map = 
	{
		0x6C9E9430,
		"DownloadFileHeader",
		L"DownloadFileHeaderResponse",
		sizeof("DownloadFileHeader")-1,
		sizeof("DownloadFileHeaderResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownloadFileHeader_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryLikeClients_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryLikeClients_atlsoapheader_map = 
	{
		0x5623EA5C,
		"QueryLikeClients",
		L"QueryLikeClientsResponse",
		sizeof("QueryLikeClients")-1,
		sizeof("QueryLikeClientsResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryLikeClients_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryWorkLogByDateAndProId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryWorkLogByDateAndProId_atlsoapheader_map = 
	{
		0x18F60CB4,
		"QueryWorkLogByDateAndProId",
		L"QueryWorkLogByDateAndProIdResponse",
		sizeof("QueryWorkLogByDateAndProId")-1,
		sizeof("QueryWorkLogByDateAndProIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryWorkLogByDateAndProId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownloadPartBuildById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownloadPartBuildById_atlsoapheader_map = 
	{
		0xF2F18FF6,
		"DownloadPartBuildById",
		L"DownloadPartBuildByIdResponse",
		sizeof("DownloadPartBuildById")-1,
		sizeof("DownloadPartBuildByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownloadPartBuildById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_LogoutUser_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_LogoutUser_atlsoapheader_map = 
	{
		0xDD147D88,
		"LogoutUser",
		L"LogoutUserResponse",
		sizeof("LogoutUser")-1,
		sizeof("LogoutUserResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_LogoutUser_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryLastReplyByFeedBackId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryLastReplyByFeedBackId_atlsoapheader_map = 
	{
		0x487948B2,
		"QueryLastReplyByFeedBackId",
		L"QueryLastReplyByFeedBackIdResponse",
		sizeof("QueryLastReplyByFeedBackId")-1,
		sizeof("QueryLastReplyByFeedBackIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryLastReplyByFeedBackId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownLoadClientById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownLoadClientById_atlsoapheader_map = 
	{
		0x8607AC6E,
		"DownLoadClientById",
		L"DownLoadClientByIdResponse",
		sizeof("DownLoadClientById")-1,
		sizeof("DownLoadClientByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownLoadClientById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryGroupFeedBacks_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryGroupFeedBacks_atlsoapheader_map = 
	{
		0x0A0EC26A,
		"QueryGroupFeedBacks",
		L"QueryGroupFeedBacksResponse",
		sizeof("QueryGroupFeedBacks")-1,
		sizeof("QueryGroupFeedBacksResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryGroupFeedBacks_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateFeedbackGroup_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateFeedbackGroup_atlsoapheader_map = 
	{
		0xB12EDC34,
		"SaveOrUpdateFeedbackGroup",
		L"SaveOrUpdateFeedbackGroupResponse",
		sizeof("SaveOrUpdateFeedbackGroup")-1,
		sizeof("SaveOrUpdateFeedbackGroupResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateFeedbackGroup_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryUserByRoleId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryUserByRoleId_atlsoapheader_map = 
	{
		0xB3A0D71E,
		"QueryUserByRoleId",
		L"QueryUserByRoleIdResponse",
		sizeof("QueryUserByRoleId")-1,
		sizeof("QueryUserByRoleIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryUserByRoleId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteClient_ProductionByProduct_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteClient_ProductionByProduct_atlsoapheader_map = 
	{
		0x760FAB63,
		"DeleteClient_ProductionByProduct",
		L"DeleteClient_ProductionByProductResponse",
		sizeof("DeleteClient_ProductionByProduct")-1,
		sizeof("DeleteClient_ProductionByProductResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteClient_ProductionByProduct_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateFeedBack_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateFeedBack_atlsoapheader_map = 
	{
		0x22484567,
		"SaveOrUpdateFeedBack",
		L"SaveOrUpdateFeedBackResponse",
		sizeof("SaveOrUpdateFeedBack")-1,
		sizeof("SaveOrUpdateFeedBackResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateFeedBack_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteUser_RoleByUserIdAndRoleId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteUser_RoleByUserIdAndRoleId_atlsoapheader_map = 
	{
		0xA7AF13EB,
		"DeleteUser_RoleByUserIdAndRoleId",
		L"DeleteUser_RoleByUserIdAndRoleIdResponse",
		sizeof("DeleteUser_RoleByUserIdAndRoleId")-1,
		sizeof("DeleteUser_RoleByUserIdAndRoleIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteUser_RoleByUserIdAndRoleId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateTestItemSet_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateTestItemSet_atlsoapheader_map = 
	{
		0x2BB8BB9D,
		"SaveOrUpdateTestItemSet",
		L"SaveOrUpdateTestItemSetResponse",
		sizeof("SaveOrUpdateTestItemSet")-1,
		sizeof("SaveOrUpdateTestItemSetResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateTestItemSet_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_ClearUserPassword_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_ClearUserPassword_atlsoapheader_map = 
	{
		0x620A02E8,
		"ClearUserPassword",
		L"ClearUserPasswordResponse",
		sizeof("ClearUserPassword")-1,
		sizeof("ClearUserPasswordResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_ClearUserPassword_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryAllAttendance_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryAllAttendance_atlsoapheader_map = 
	{
		0xC745A955,
		"QueryAllAttendance",
		L"QueryAllAttendanceResponse",
		sizeof("QueryAllAttendance")-1,
		sizeof("QueryAllAttendanceResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryAllAttendance_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteDeveLop_testItemByItemId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteDeveLop_testItemByItemId_atlsoapheader_map = 
	{
		0x9F88D656,
		"DeleteDeveLop_testItemByItemId",
		L"DeleteDeveLop_testItemByItemIdResponse",
		sizeof("DeleteDeveLop_testItemByItemId")-1,
		sizeof("DeleteDeveLop_testItemByItemIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteDeveLop_testItemByItemId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryWorkLogByUserId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryWorkLogByUserId_atlsoapheader_map = 
	{
		0x64629971,
		"QueryWorkLogByUserId",
		L"QueryWorkLogByUserIdResponse",
		sizeof("QueryWorkLogByUserId")-1,
		sizeof("QueryWorkLogByUserIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryWorkLogByUserId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryFeedBackArrBySubmitDate_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryFeedBackArrBySubmitDate_atlsoapheader_map = 
	{
		0x4D63FFBC,
		"QueryFeedBackArrBySubmitDate",
		L"QueryFeedBackArrBySubmitDateResponse",
		sizeof("QueryFeedBackArrBySubmitDate")-1,
		sizeof("QueryFeedBackArrBySubmitDateResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryFeedBackArrBySubmitDate_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownloadTestItemById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownloadTestItemById_atlsoapheader_map = 
	{
		0x890CBD3E,
		"DownloadTestItemById",
		L"DownloadTestItemByIdResponse",
		sizeof("DownloadTestItemById")-1,
		sizeof("DownloadTestItemByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownloadTestItemById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_ValidatePassword_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_ValidatePassword_atlsoapheader_map = 
	{
		0x87E40AAC,
		"ValidatePassword",
		L"ValidatePasswordResponse",
		sizeof("ValidatePassword")-1,
		sizeof("ValidatePasswordResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_ValidatePassword_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryTestItemByWorkLogId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryTestItemByWorkLogId_atlsoapheader_map = 
	{
		0x5F4F6021,
		"QueryTestItemByWorkLogId",
		L"QueryTestItemByWorkLogIdResponse",
		sizeof("QueryTestItemByWorkLogId")-1,
		sizeof("QueryTestItemByWorkLogIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryTestItemByWorkLogId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryNotificationArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryNotificationArr_atlsoapheader_map = 
	{
		0x5E6F7A11,
		"QueryNotificationArr",
		L"QueryNotificationArrResponse",
		sizeof("QueryNotificationArr")-1,
		sizeof("QueryNotificationArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryNotificationArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_GetRelationObject_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_GetRelationObject_atlsoapheader_map = 
	{
		0x851F75A4,
		"GetRelationObject",
		L"GetRelationObjectResponse",
		sizeof("GetRelationObject")-1,
		sizeof("GetRelationObjectResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_GetRelationObject_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeletePartBuildByPartItemId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeletePartBuildByPartItemId_atlsoapheader_map = 
	{
		0x487C5357,
		"DeletePartBuildByPartItemId",
		L"DeletePartBuildByPartItemIdResponse",
		sizeof("DeletePartBuildByPartItemId")-1,
		sizeof("DeletePartBuildByPartItemIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeletePartBuildByPartItemId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownLoadMemberRole_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownLoadMemberRole_atlsoapheader_map = 
	{
		0x3D731251,
		"DownLoadMemberRole",
		L"DownLoadMemberRoleResponse",
		sizeof("DownLoadMemberRole")-1,
		sizeof("DownLoadMemberRoleResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownLoadMemberRole_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteProblemGradeById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteProblemGradeById_atlsoapheader_map = 
	{
		0x13712BBE,
		"DeleteProblemGradeById",
		L"DeleteProblemGradeByIdResponse",
		sizeof("DeleteProblemGradeById")-1,
		sizeof("DeleteProblemGradeByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteProblemGradeById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryAllDownloadItem_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryAllDownloadItem_atlsoapheader_map = 
	{
		0x74007E05,
		"QueryAllDownloadItem",
		L"QueryAllDownloadItemResponse",
		sizeof("QueryAllDownloadItem")-1,
		sizeof("QueryAllDownloadItemResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryAllDownloadItem_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryAllNews_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryAllNews_atlsoapheader_map = 
	{
		0x522FADBB,
		"QueryAllNews",
		L"QueryAllNewsResponse",
		sizeof("QueryAllNews")-1,
		sizeof("QueryAllNewsResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryAllNews_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_GetNotification_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_GetNotification_atlsoapheader_map = 
	{
		0x20A8A116,
		"GetNotification",
		L"GetNotificationResponse",
		sizeof("GetNotification")-1,
		sizeof("GetNotificationResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_GetNotification_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateDevelopLog_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateDevelopLog_atlsoapheader_map = 
	{
		0x7654CA13,
		"SaveOrUpdateDevelopLog",
		L"SaveOrUpdateDevelopLogResponse",
		sizeof("SaveOrUpdateDevelopLog")-1,
		sizeof("SaveOrUpdateDevelopLogResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateDevelopLog_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryClientArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryClientArr_atlsoapheader_map = 
	{
		0x08B432C9,
		"QueryClientArr",
		L"QueryClientArrResponse",
		sizeof("QueryClientArr")-1,
		sizeof("QueryClientArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryClientArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateWorkLog_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateWorkLog_atlsoapheader_map = 
	{
		0x2F692007,
		"SaveOrUpdateWorkLog",
		L"SaveOrUpdateWorkLogResponse",
		sizeof("SaveOrUpdateWorkLog")-1,
		sizeof("SaveOrUpdateWorkLogResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateWorkLog_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryCreateTaskUsers_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryCreateTaskUsers_atlsoapheader_map = 
	{
		0x41D8077E,
		"QueryCreateTaskUsers",
		L"QueryCreateTaskUsersResponse",
		sizeof("QueryCreateTaskUsers")-1,
		sizeof("QueryCreateTaskUsersResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryCreateTaskUsers_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteClient_ProductionByClient_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteClient_ProductionByClient_atlsoapheader_map = 
	{
		0xE891C6E1,
		"DeleteClient_ProductionByClient",
		L"DeleteClient_ProductionByClientResponse",
		sizeof("DeleteClient_ProductionByClient")-1,
		sizeof("DeleteClient_ProductionByClientResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteClient_ProductionByClient_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateReply_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateReply_atlsoapheader_map = 
	{
		0xDA2A394E,
		"SaveOrUpdateReply",
		L"SaveOrUpdateReplyResponse",
		sizeof("SaveOrUpdateReply")-1,
		sizeof("SaveOrUpdateReplyResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateReply_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteProduction_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteProduction_atlsoapheader_map = 
	{
		0x0A788609,
		"DeleteProduction",
		L"DeleteProductionResponse",
		sizeof("DeleteProduction")-1,
		sizeof("DeleteProductionResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteProduction_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_GetProduction_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_GetProduction_atlsoapheader_map = 
	{
		0x9D98BF36,
		"GetProduction",
		L"GetProductionResponse",
		sizeof("GetProduction")-1,
		sizeof("GetProductionResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_GetProduction_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownLoadTechnoiogyRole_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownLoadTechnoiogyRole_atlsoapheader_map = 
	{
		0xC39ED552,
		"DownLoadTechnoiogyRole",
		L"DownLoadTechnoiogyRoleResponse",
		sizeof("DownLoadTechnoiogyRole")-1,
		sizeof("DownLoadTechnoiogyRoleResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownLoadTechnoiogyRole_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryFeedBackArrByVersionRevision_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryFeedBackArrByVersionRevision_atlsoapheader_map = 
	{
		0x09C5B2FF,
		"QueryFeedBackArrByVersionRevision",
		L"QueryFeedBackArrByVersionRevisionResponse",
		sizeof("QueryFeedBackArrByVersionRevision")-1,
		sizeof("QueryFeedBackArrByVersionRevisionResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryFeedBackArrByVersionRevision_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateAttendance_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateAttendance_atlsoapheader_map = 
	{
		0xFA6D4B19,
		"SaveOrUpdateAttendance",
		L"SaveOrUpdateAttendanceResponse",
		sizeof("SaveOrUpdateAttendance")-1,
		sizeof("SaveOrUpdateAttendanceResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateAttendance_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteClientDownloadByClientId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteClientDownloadByClientId_atlsoapheader_map = 
	{
		0x73653E00,
		"DeleteClientDownloadByClientId",
		L"DeleteClientDownloadByClientIdResponse",
		sizeof("DeleteClientDownloadByClientId")-1,
		sizeof("DeleteClientDownloadByClientIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteClientDownloadByClientId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdateRole_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdateRole_atlsoapheader_map = 
	{
		0xECEC6C14,
		"SaveOrUpdateRole",
		L"SaveOrUpdateRoleResponse",
		sizeof("SaveOrUpdateRole")-1,
		sizeof("SaveOrUpdateRoleResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdateRole_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryAllFeedBackArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryAllFeedBackArr_atlsoapheader_map = 
	{
		0x2A1D38C8,
		"QueryAllFeedBackArr",
		L"QueryAllFeedBackArrResponse",
		sizeof("QueryAllFeedBackArr")-1,
		sizeof("QueryAllFeedBackArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryAllFeedBackArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteFeedBackGroup_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteFeedBackGroup_atlsoapheader_map = 
	{
		0x6A8639D4,
		"DeleteFeedBackGroup",
		L"DeleteFeedBackGroupResponse",
		sizeof("DeleteFeedBackGroup")-1,
		sizeof("DeleteFeedBackGroupResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteFeedBackGroup_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SaveOrUpdatePartBuild_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SaveOrUpdatePartBuild_atlsoapheader_map = 
	{
		0x661D5669,
		"SaveOrUpdatePartBuild",
		L"SaveOrUpdatePartBuildResponse",
		sizeof("SaveOrUpdatePartBuild")-1,
		sizeof("SaveOrUpdatePartBuildResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SaveOrUpdatePartBuild_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteDevelopLopById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteDevelopLopById_atlsoapheader_map = 
	{
		0xFF9C0CE4,
		"DeleteDevelopLopById",
		L"DeleteDevelopLopByIdResponse",
		sizeof("DeleteDevelopLopById")-1,
		sizeof("DeleteDevelopLopByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteDevelopLopById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownloadWorkLogById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownloadWorkLogById_atlsoapheader_map = 
	{
		0x86257354,
		"DownloadWorkLogById",
		L"DownloadWorkLogByIdResponse",
		sizeof("DownloadWorkLogById")-1,
		sizeof("DownloadWorkLogByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownloadWorkLogById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryAttendanceArrByUserId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryAttendanceArrByUserId_atlsoapheader_map = 
	{
		0xA40EDA28,
		"QueryAttendanceArrByUserId",
		L"QueryAttendanceArrByUserIdResponse",
		sizeof("QueryAttendanceArrByUserId")-1,
		sizeof("QueryAttendanceArrByUserIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryAttendanceArrByUserId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteJobById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteJobById_atlsoapheader_map = 
	{
		0xAEC34CE5,
		"DeleteJobById",
		L"DeleteJobByIdResponse",
		sizeof("DeleteJobById")-1,
		sizeof("DeleteJobByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteJobById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_UpdateTasksSerialNo_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_UpdateTasksSerialNo_atlsoapheader_map = 
	{
		0x8AAF85B5,
		"UpdateTasksSerialNo",
		L"UpdateTasksSerialNoResponse",
		sizeof("UpdateTasksSerialNo")-1,
		sizeof("UpdateTasksSerialNoResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_UpdateTasksSerialNo_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryAllUser_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryAllUser_atlsoapheader_map = 
	{
		0x36AE7F9D,
		"QueryAllUser",
		L"QueryAllUserResponse",
		sizeof("QueryAllUser")-1,
		sizeof("QueryAllUserResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryAllUser_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_canExecuteFuncOper_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_canExecuteFuncOper_atlsoapheader_map = 
	{
		0xCB3F3D96,
		"canExecuteFuncOper",
		L"canExecuteFuncOperResponse",
		sizeof("canExecuteFuncOper")-1,
		sizeof("canExecuteFuncOperResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_canExecuteFuncOper_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteFeedBack_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteFeedBack_atlsoapheader_map = 
	{
		0xD2F24B27,
		"DeleteFeedBack",
		L"DeleteFeedBackResponse",
		sizeof("DeleteFeedBack")-1,
		sizeof("DeleteFeedBackResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteFeedBack_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeletePartBuildById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeletePartBuildById_atlsoapheader_map = 
	{
		0x2213C6B1,
		"DeletePartBuildById",
		L"DeletePartBuildByIdResponse",
		sizeof("DeletePartBuildById")-1,
		sizeof("DeletePartBuildByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeletePartBuildById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryTestItemByVersionId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryTestItemByVersionId_atlsoapheader_map = 
	{
		0xCFB0D8C2,
		"QueryTestItemByVersionId",
		L"QueryTestItemByVersionIdResponse",
		sizeof("QueryTestItemByVersionId")-1,
		sizeof("QueryTestItemByVersionIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryTestItemByVersionId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryVersionUpdateInfoArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryVersionUpdateInfoArr_atlsoapheader_map = 
	{
		0x61C9035F,
		"QueryVersionUpdateInfoArr",
		L"QueryVersionUpdateInfoArrResponse",
		sizeof("QueryVersionUpdateInfoArr")-1,
		sizeof("QueryVersionUpdateInfoArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryVersionUpdateInfoArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryCategoryArr_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryCategoryArr_atlsoapheader_map = 
	{
		0x2EB78088,
		"QueryCategoryArr",
		L"QueryCategoryArrResponse",
		sizeof("QueryCategoryArr")-1,
		sizeof("QueryCategoryArrResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryCategoryArr_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryFeedBacks_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryFeedBacks_atlsoapheader_map = 
	{
		0x6B8CDA5D,
		"QueryFeedBacks",
		L"QueryFeedBacksResponse",
		sizeof("QueryFeedBacks")-1,
		sizeof("QueryFeedBacksResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryFeedBacks_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_InserClientDownloadItem_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_InserClientDownloadItem_atlsoapheader_map = 
	{
		0x5BDA25F6,
		"InserClientDownloadItem",
		L"InserClientDownloadItemResponse",
		sizeof("InserClientDownloadItem")-1,
		sizeof("InserClientDownloadItemResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_InserClientDownloadItem_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteVersionRevision_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteVersionRevision_atlsoapheader_map = 
	{
		0xD073DE77,
		"DeleteVersionRevision",
		L"DeleteVersionRevisionResponse",
		sizeof("DeleteVersionRevision")-1,
		sizeof("DeleteVersionRevisionResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteVersionRevision_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_UpdateWorklog_testItem_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_UpdateWorklog_testItem_atlsoapheader_map = 
	{
		0xBAF749E5,
		"UpdateWorklog_testItem",
		L"UpdateWorklog_testItemResponse",
		sizeof("UpdateWorklog_testItem")-1,
		sizeof("UpdateWorklog_testItemResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_UpdateWorklog_testItem_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryTaskDispatchArrByReceiverId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryTaskDispatchArrByReceiverId_atlsoapheader_map = 
	{
		0xD7F9F24A,
		"QueryTaskDispatchArrByReceiverId",
		L"QueryTaskDispatchArrByReceiverIdResponse",
		sizeof("QueryTaskDispatchArrByReceiverId")-1,
		sizeof("QueryTaskDispatchArrByReceiverIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryTaskDispatchArrByReceiverId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DeleteNewsById_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DeleteNewsById_atlsoapheader_map = 
	{
		0xEDE83FC7,
		"DeleteNewsById",
		L"DeleteNewsByIdResponse",
		sizeof("DeleteNewsById")-1,
		sizeof("DeleteNewsByIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DeleteNewsById_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownloadUser_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownloadUser_atlsoapheader_map = 
	{
		0xD721A3C6,
		"DownloadUser",
		L"DownloadUserResponse",
		sizeof("DownloadUser")-1,
		sizeof("DownloadUserResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownloadUser_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_LoginUser_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_LoginUser_atlsoapheader_map = 
	{
		0xBEE94A07,
		"LoginUser",
		L"LoginUserResponse",
		sizeof("LoginUser")-1,
		sizeof("LoginUserResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_LoginUser_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryAllPartBuild_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryAllPartBuild_atlsoapheader_map = 
	{
		0x3604DD25,
		"QueryAllPartBuild",
		L"QueryAllPartBuildResponse",
		sizeof("QueryAllPartBuild")-1,
		sizeof("QueryAllPartBuildResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryAllPartBuild_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryVersionTestItemArrByVersionId_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryVersionTestItemArrByVersionId_atlsoapheader_map = 
	{
		0x74A9EEAD,
		"QueryVersionTestItemArrByVersionId",
		L"QueryVersionTestItemArrByVersionIdResponse",
		sizeof("QueryVersionTestItemArrByVersionId")-1,
		sizeof("QueryVersionTestItemArrByVersionIdResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryVersionTestItemArrByVersionId_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_GetFeedBack_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_GetFeedBack_atlsoapheader_map = 
	{
		0x30685514,
		"GetFeedBack",
		L"GetFeedBackResponse",
		sizeof("GetFeedBack")-1,
		sizeof("GetFeedBackResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_GetFeedBack_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_UpdateVersion_TestItem_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_UpdateVersion_TestItem_atlsoapheader_map = 
	{
		0x39DE1B26,
		"UpdateVersion_TestItem",
		L"UpdateVersion_TestItemResponse",
		sizeof("UpdateVersion_TestItem")-1,
		sizeof("UpdateVersion_TestItemResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_UpdateVersion_TestItem_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_DownloadProblemGrade_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_DownloadProblemGrade_atlsoapheader_map = 
	{
		0x02622F5B,
		"DownloadProblemGrade",
		L"DownloadProblemGradeResponse",
		sizeof("DownloadProblemGrade")-1,
		sizeof("DownloadProblemGradeResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_DownloadProblemGrade_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_SetSqlLink_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_SetSqlLink_atlsoapheader_map = 
	{
		0x04E0F759,
		"SetSqlLink",
		L"SetSqlLinkResponse",
		sizeof("SetSqlLink")-1,
		sizeof("SetSqlLinkResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_SetSqlLink_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CXMServer_QueryNewsArrByDate_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CXMServer_QueryNewsArrByDate_atlsoapheader_map = 
	{
		0xA18E2820,
		"QueryNewsArrByDate",
		L"QueryNewsArrByDateResponse",
		sizeof("QueryNewsArrByDate")-1,
		sizeof("QueryNewsArrByDateResponse")-1,
		SOAPMAP_HEADER,
		__CXMServer_QueryNewsArrByDate_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};


	static const _soapmap * __CXMServer_headers[] =
	{
		&__CXMServer_GetUser_atlsoapheader_map,
		&__CXMServer_DownloadNotification_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateTaskDispatch_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateDownloadItem_atlsoapheader_map,
		&__CXMServer_QueryFeedBackArrByProblemType_atlsoapheader_map,
		&__CXMServer_QueryProblemGradeArr_atlsoapheader_map,
		&__CXMServer_UpdateUser_atlsoapheader_map,
		&__CXMServer_DownLoadNews_atlsoapheader_map,
		&__CXMServer_QueryProductionArr_atlsoapheader_map,
		&__CXMServer_QueryAllRole_atlsoapheader_map,
		&__CXMServer_InsertUser_atlsoapheader_map,
		&__CXMServer_UpdateUserTask_atlsoapheader_map,
		&__CXMServer_QueryChildTaskDispatchsByGroupId_atlsoapheader_map,
		&__CXMServer_UpdateTestItemArr_atlsoapheader_map,
		&__CXMServer_GetVersionRevision_atlsoapheader_map,
		&__CXMServer_DeleteCategoryById_atlsoapheader_map,
		&__CXMServer_QueryVersionArr_atlsoapheader_map,
		&__CXMServer_QueryTestItemSetArrByProId_atlsoapheader_map,
		&__CXMServer_GetFileHeader_atlsoapheader_map,
		&__CXMServer_GetReply_atlsoapheader_map,
		&__CXMServer_ValidateUserPerm_atlsoapheader_map,
		&__CXMServer_UpdateTaskDispatchArr_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateProduction_atlsoapheader_map,
		&__CXMServer_InsertFeedBackArr_atlsoapheader_map,
		&__CXMServer_DownloadProduction_atlsoapheader_map,
		&__CXMServer_DeleteWorkLog_atlsoapheader_map,
		&__CXMServer_QueryTestItemArrBySetId_atlsoapheader_map,
		&__CXMServer_QueryClientProductByClientId_atlsoapheader_map,
		&__CXMServer_QueryProductionByUserId_atlsoapheader_map,
		&__CXMServer_QueryTestItemArrByCondition_atlsoapheader_map,
		&__CXMServer_QueryproductionsByClientId_atlsoapheader_map,
		&__CXMServer_DownloadFeedBack_atlsoapheader_map,
		&__CXMServer_GetWorkLog_atlsoapheader_map,
		&__CXMServer_DeleteClientById_atlsoapheader_map,
		&__CXMServer_DeleteTaskDispatch_atlsoapheader_map,
		&__CXMServer_GetRole_atlsoapheader_map,
		&__CXMServer_QueryDownloadItemByClient_atlsoapheader_map,
		&__CXMServer_DeleteDownloadItem_atlsoapheader_map,
		&__CXMServer_DeleteClient_DownloadItemByItem_atlsoapheader_map,
		&__CXMServer_QueryEssentialFeedBacks_atlsoapheader_map,
		&__CXMServer_QueryFeedBackArrByProcessState_atlsoapheader_map,
		&__CXMServer_QueryCategoryArrByObjType_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateJob_atlsoapheader_map,
		&__CXMServer_QueryFeedBacksToDataSet_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateNotification_atlsoapheader_map,
		&__CXMServer_InsertUser_Role_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateVersionRevision_atlsoapheader_map,
		&__CXMServer_QueryTestItemByWorkLogDate_atlsoapheader_map,
		&__CXMServer_QueryFeedBackArrByName_atlsoapheader_map,
		&__CXMServer_DeleteUser_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateCategory_atlsoapheader_map,
		&__CXMServer_DownloadClientProductById_atlsoapheader_map,
		&__CXMServer_QueryLatestVersionRevisionArr_atlsoapheader_map,
		&__CXMServer_DeleteReply_atlsoapheader_map,
		&__CXMServer_QueryTaskDispatchs_atlsoapheader_map,
		&__CXMServer_DownloadAttendanceById_atlsoapheader_map,
		&__CXMServer_DownloadCategoryById_atlsoapheader_map,
		&__CXMServer_QueryDevelopLogArr_atlsoapheader_map,
		&__CXMServer_DownloadItemById_atlsoapheader_map,
		&__CXMServer_InsertTaskDispatchArr_atlsoapheader_map,
		&__CXMServer_DownLoadLasrVersionByProId_atlsoapheader_map,
		&__CXMServer_QueryClientByProvinceNum_atlsoapheader_map,
		&__CXMServer_DeleteWorkLogById_atlsoapheader_map,
		&__CXMServer_QueryFeedBackArrByProposerId_atlsoapheader_map,
		&__CXMServer_DownLoadJobById_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateTestItem_atlsoapheader_map,
		&__CXMServer_QueryAllJobs_atlsoapheader_map,
		&__CXMServer_QueryPartBuildByPartItemId_atlsoapheader_map,
		&__CXMServer_QueryRolesByUserId_atlsoapheader_map,
		&__CXMServer_QueryFeedBackArrByProblemGrade_atlsoapheader_map,
		&__CXMServer_DeleteTestItemSet_atlsoapheader_map,
		&__CXMServer_QueryFeedBackArrByCondition_atlsoapheader_map,
		&__CXMServer_DownloadVersionRevision_atlsoapheader_map,
		&__CXMServer_DeleteAttendance_atlsoapheader_map,
		&__CXMServer_ValidateUserName_atlsoapheader_map,
		&__CXMServer_DeleteTestItemById_atlsoapheader_map,
		&__CXMServer_DownloadTaskDispatch_atlsoapheader_map,
		&__CXMServer_QueryFeedBacksCount_atlsoapheader_map,
		&__CXMServer_QueryInsideUsers_atlsoapheader_map,
		&__CXMServer_GetLoginStruct_atlsoapheader_map,
		&__CXMServer_QueryTestItemArr_atlsoapheader_map,
		&__CXMServer_DeleteNotification_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateNews_atlsoapheader_map,
		&__CXMServer_QueryVersionRevisionArrByProductionId_atlsoapheader_map,
		&__CXMServer_QueryFeedBackArrByProductionId_atlsoapheader_map,
		&__CXMServer_DeleteDeveLop_testItemByDevelopLopId_atlsoapheader_map,
		&__CXMServer_DeleteClient_DownloadItemByClient_atlsoapheader_map,
		&__CXMServer_QueryAttendanceByDate_atlsoapheader_map,
		&__CXMServer_GetTaskDispatch_atlsoapheader_map,
		&__CXMServer_DeleteRole_atlsoapheader_map,
		&__CXMServer_UpdateVersion_TestItemArr_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateClient_atlsoapheader_map,
		&__CXMServer_QueryAllDevelopLog_atlsoapheader_map,
		&__CXMServer_UpdateFeedBacksSerialNo_atlsoapheader_map,
		&__CXMServer_DownloadFileHeader_atlsoapheader_map,
		&__CXMServer_QueryLikeClients_atlsoapheader_map,
		&__CXMServer_QueryWorkLogByDateAndProId_atlsoapheader_map,
		&__CXMServer_DownloadPartBuildById_atlsoapheader_map,
		&__CXMServer_LogoutUser_atlsoapheader_map,
		&__CXMServer_QueryLastReplyByFeedBackId_atlsoapheader_map,
		&__CXMServer_DownLoadClientById_atlsoapheader_map,
		&__CXMServer_QueryGroupFeedBacks_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateFeedbackGroup_atlsoapheader_map,
		&__CXMServer_QueryUserByRoleId_atlsoapheader_map,
		&__CXMServer_DeleteClient_ProductionByProduct_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateFeedBack_atlsoapheader_map,
		&__CXMServer_DeleteUser_RoleByUserIdAndRoleId_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateTestItemSet_atlsoapheader_map,
		&__CXMServer_ClearUserPassword_atlsoapheader_map,
		&__CXMServer_QueryAllAttendance_atlsoapheader_map,
		&__CXMServer_DeleteDeveLop_testItemByItemId_atlsoapheader_map,
		&__CXMServer_QueryWorkLogByUserId_atlsoapheader_map,
		&__CXMServer_QueryFeedBackArrBySubmitDate_atlsoapheader_map,
		&__CXMServer_DownloadTestItemById_atlsoapheader_map,
		&__CXMServer_ValidatePassword_atlsoapheader_map,
		&__CXMServer_QueryTestItemByWorkLogId_atlsoapheader_map,
		&__CXMServer_QueryNotificationArr_atlsoapheader_map,
		&__CXMServer_GetRelationObject_atlsoapheader_map,
		&__CXMServer_DeletePartBuildByPartItemId_atlsoapheader_map,
		&__CXMServer_DownLoadMemberRole_atlsoapheader_map,
		&__CXMServer_DeleteProblemGradeById_atlsoapheader_map,
		&__CXMServer_QueryAllDownloadItem_atlsoapheader_map,
		&__CXMServer_QueryAllNews_atlsoapheader_map,
		&__CXMServer_GetNotification_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateDevelopLog_atlsoapheader_map,
		&__CXMServer_QueryClientArr_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateWorkLog_atlsoapheader_map,
		&__CXMServer_QueryCreateTaskUsers_atlsoapheader_map,
		&__CXMServer_DeleteClient_ProductionByClient_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateReply_atlsoapheader_map,
		&__CXMServer_DeleteProduction_atlsoapheader_map,
		&__CXMServer_GetProduction_atlsoapheader_map,
		&__CXMServer_DownLoadTechnoiogyRole_atlsoapheader_map,
		&__CXMServer_QueryFeedBackArrByVersionRevision_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateAttendance_atlsoapheader_map,
		&__CXMServer_DeleteClientDownloadByClientId_atlsoapheader_map,
		&__CXMServer_SaveOrUpdateRole_atlsoapheader_map,
		&__CXMServer_QueryAllFeedBackArr_atlsoapheader_map,
		&__CXMServer_DeleteFeedBackGroup_atlsoapheader_map,
		&__CXMServer_SaveOrUpdatePartBuild_atlsoapheader_map,
		&__CXMServer_DeleteDevelopLopById_atlsoapheader_map,
		&__CXMServer_DownloadWorkLogById_atlsoapheader_map,
		&__CXMServer_QueryAttendanceArrByUserId_atlsoapheader_map,
		&__CXMServer_DeleteJobById_atlsoapheader_map,
		&__CXMServer_UpdateTasksSerialNo_atlsoapheader_map,
		&__CXMServer_QueryAllUser_atlsoapheader_map,
		&__CXMServer_canExecuteFuncOper_atlsoapheader_map,
		&__CXMServer_DeleteFeedBack_atlsoapheader_map,
		&__CXMServer_DeletePartBuildById_atlsoapheader_map,
		&__CXMServer_QueryTestItemByVersionId_atlsoapheader_map,
		&__CXMServer_QueryVersionUpdateInfoArr_atlsoapheader_map,
		&__CXMServer_QueryCategoryArr_atlsoapheader_map,
		&__CXMServer_QueryFeedBacks_atlsoapheader_map,
		&__CXMServer_InserClientDownloadItem_atlsoapheader_map,
		&__CXMServer_DeleteVersionRevision_atlsoapheader_map,
		&__CXMServer_UpdateWorklog_testItem_atlsoapheader_map,
		&__CXMServer_QueryTaskDispatchArrByReceiverId_atlsoapheader_map,
		&__CXMServer_DeleteNewsById_atlsoapheader_map,
		&__CXMServer_DownloadUser_atlsoapheader_map,
		&__CXMServer_LoginUser_atlsoapheader_map,
		&__CXMServer_QueryAllPartBuild_atlsoapheader_map,
		&__CXMServer_QueryVersionTestItemArrByVersionId_atlsoapheader_map,
		&__CXMServer_GetFeedBack_atlsoapheader_map,
		&__CXMServer_UpdateVersion_TestItem_atlsoapheader_map,
		&__CXMServer_DownloadProblemGrade_atlsoapheader_map,
		&__CXMServer_SetSqlLink_atlsoapheader_map,
		&__CXMServer_QueryNewsArrByDate_atlsoapheader_map,
		NULL
	};
	
	return __CXMServer_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CXMServerT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CXMServerT<TClient>::GetNamespaceUri()
{
	return L"http://tempuri.org/";
}

template <typename TClient>
ATL_NOINLINE inline const char * CXMServerT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CXMServerT<TClient>::GetNamespaceUriA()
{
	return "http://tempuri.org/";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CXMServerT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CXMServerT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
{
	if (ppReader == NULL)
	{
		return E_INVALIDARG;
	}
	
	CComPtr<ISAXXMLReader> spReader = GetReader();
	if (spReader.p != NULL)
	{
		*ppReader = spReader.Detach();
		return S_OK;
	}
	return TClient::GetClientReader(ppReader);
}

} // namespace XMServer

template<>
inline HRESULT AtlCleanupValue<XMServer::Function>(XMServer::Function *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->functionId);
	AtlCleanupValue(&pVal->functdesc);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::Function>(XMServer::Function *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->functionId, pMemMgr);
	AtlCleanupValueEx(&pVal->functdesc, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::RelationObject>(XMServer::RelationObject *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->relationId);
	AtlCleanupValue(&pVal->relationName);
	AtlCleanupValue(&pVal->relationObj);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::RelationObject>(XMServer::RelationObject *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->relationId, pMemMgr);
	AtlCleanupValueEx(&pVal->relationName, pMemMgr);
	AtlCleanupValueEx(&pVal->relationObj, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::TestItem>(XMServer::TestItem *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->share);
	AtlCleanupValue(&pVal->title);
	AtlCleanupValue(&pVal->description);
	AtlCleanupValue(&pVal->ProductFlag);
	AtlCleanupValue(&pVal->categoryObj);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::TestItem>(XMServer::TestItem *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->share, pMemMgr);
	AtlCleanupValueEx(&pVal->title, pMemMgr);
	AtlCleanupValueEx(&pVal->description, pMemMgr);
	AtlCleanupValueEx(&pVal->ProductFlag, pMemMgr);
	AtlCleanupValueEx(&pVal->categoryObj, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::DevelopLog>(XMServer::DevelopLog *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->title);
	AtlCleanupValue(&pVal->description);
	AtlCleanupValue(&pVal->startDate);
	AtlCleanupValue(&pVal->endDate);
	AtlCleanupValue(&pVal->user);
	AtlCleanupValue(&pVal->production);
	AtlCleanupValue(&pVal->relateFile);
	AtlCleanupArray(pVal->testItemArr, pVal->__testItemArr_nSizeIs);
	AtlCleanupValue(&pVal->tag);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::DevelopLog>(XMServer::DevelopLog *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->title, pMemMgr);
	AtlCleanupValueEx(&pVal->description, pMemMgr);
	AtlCleanupValueEx(&pVal->startDate, pMemMgr);
	AtlCleanupValueEx(&pVal->endDate, pMemMgr);
	AtlCleanupValueEx(&pVal->user, pMemMgr);
	AtlCleanupValueEx(&pVal->production, pMemMgr);
	AtlCleanupValueEx(&pVal->relateFile, pMemMgr);
	if (pVal->testItemArr != NULL)
	{
		AtlCleanupArrayEx(pVal->testItemArr, pVal->__testItemArr_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->testItemArr);
	}
	AtlCleanupValueEx(&pVal->tag, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::WorLog_testItem>(XMServer::WorLog_testItem *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->logId);
	AtlCleanupValue(&pVal->testId);
	AtlCleanupValue(&pVal->processed);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::WorLog_testItem>(XMServer::WorLog_testItem *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->logId, pMemMgr);
	AtlCleanupValueEx(&pVal->testId, pMemMgr);
	AtlCleanupValueEx(&pVal->processed, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::Attendance>(XMServer::Attendance *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->user);
	AtlCleanupValue(&pVal->supervisor);
	AtlCleanupValue(&pVal->submitDate);
	AtlCleanupValue(&pVal->attendanceDate);
	AtlCleanupValue(&pVal->attendanceType);
	AtlCleanupValue(&pVal->mender);
	AtlCleanupValue(&pVal->notes);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::Attendance>(XMServer::Attendance *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->user, pMemMgr);
	AtlCleanupValueEx(&pVal->supervisor, pMemMgr);
	AtlCleanupValueEx(&pVal->submitDate, pMemMgr);
	AtlCleanupValueEx(&pVal->attendanceDate, pMemMgr);
	AtlCleanupValueEx(&pVal->attendanceType, pMemMgr);
	AtlCleanupValueEx(&pVal->mender, pMemMgr);
	AtlCleanupValueEx(&pVal->notes, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::FileHeader>(XMServer::FileHeader *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->filePath);
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->fileType);
	AtlCleanupValue(&pVal->fileName);
	AtlCleanupValue(&pVal->fileNameServer);
	AtlCleanupValue(&pVal->hostId);
	AtlCleanupValue(&pVal->ownerId);
	AtlCleanupValue(&pVal->matter);
	AtlCleanupValue(&pVal->description);
	AtlCleanupValue(&pVal->tag);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::FileHeader>(XMServer::FileHeader *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->filePath, pMemMgr);
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->fileType, pMemMgr);
	AtlCleanupValueEx(&pVal->fileName, pMemMgr);
	AtlCleanupValueEx(&pVal->fileNameServer, pMemMgr);
	AtlCleanupValueEx(&pVal->hostId, pMemMgr);
	AtlCleanupValueEx(&pVal->ownerId, pMemMgr);
	AtlCleanupValueEx(&pVal->matter, pMemMgr);
	AtlCleanupValueEx(&pVal->description, pMemMgr);
	AtlCleanupValueEx(&pVal->tag, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::Reply>(XMServer::Reply *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->feedBack);
	AtlCleanupValue(&pVal->content);
	AtlCleanupValue(&pVal->processMan);
	AtlCleanupArray(pVal->attachmFileArr, pVal->__attachmFileArr_nSizeIs);
	AtlCleanupValue(&pVal->processDate);
	AtlCleanupValue(&pVal->tag);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::Reply>(XMServer::Reply *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->feedBack, pMemMgr);
	AtlCleanupValueEx(&pVal->content, pMemMgr);
	AtlCleanupValueEx(&pVal->processMan, pMemMgr);
	if (pVal->attachmFileArr != NULL)
	{
		AtlCleanupArrayEx(pVal->attachmFileArr, pVal->__attachmFileArr_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->attachmFileArr);
	}
	AtlCleanupValueEx(&pVal->processDate, pMemMgr);
	AtlCleanupValueEx(&pVal->tag, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::Client_DownloadItem>(XMServer::Client_DownloadItem *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->client_id);
	AtlCleanupValue(&pVal->downloadItem_id);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::Client_DownloadItem>(XMServer::Client_DownloadItem *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->client_id, pMemMgr);
	AtlCleanupValueEx(&pVal->downloadItem_id, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::Bug>(XMServer::Bug *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->bugId);
	AtlCleanupValue(&pVal->bugDesc);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::Bug>(XMServer::Bug *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->bugId, pMemMgr);
	AtlCleanupValueEx(&pVal->bugDesc, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::VersionInfo>(XMServer::VersionInfo *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->versionId);
	AtlCleanupValue(&pVal->version);
	AtlCleanupValue(&pVal->productionName);
	AtlCleanupValue(&pVal->promulgateDate);
	AtlCleanupArray(pVal->addFunction, pVal->__addFunction_nSizeIs);
	AtlCleanupArray(pVal->addBug, pVal->__addBug_nSizeIs);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::VersionInfo>(XMServer::VersionInfo *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->versionId, pMemMgr);
	AtlCleanupValueEx(&pVal->version, pMemMgr);
	AtlCleanupValueEx(&pVal->productionName, pMemMgr);
	AtlCleanupValueEx(&pVal->promulgateDate, pMemMgr);
	if (pVal->addFunction != NULL)
	{
		AtlCleanupArrayEx(pVal->addFunction, pVal->__addFunction_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->addFunction);
	}
	if (pVal->addBug != NULL)
	{
		AtlCleanupArrayEx(pVal->addBug, pVal->__addBug_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->addBug);
	}
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::DownloadItem>(XMServer::DownloadItem *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->file_name);
	AtlCleanupValue(&pVal->title);
	AtlCleanupValue(&pVal->updateDate);
	AtlCleanupValue(&pVal->notes);
	AtlCleanupValue(&pVal->file);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::DownloadItem>(XMServer::DownloadItem *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->file_name, pMemMgr);
	AtlCleanupValueEx(&pVal->title, pMemMgr);
	AtlCleanupValueEx(&pVal->updateDate, pMemMgr);
	AtlCleanupValueEx(&pVal->notes, pMemMgr);
	AtlCleanupValueEx(&pVal->file, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::TaskDispatch>(XMServer::TaskDispatch *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->acceptUser);
	AtlCleanupValue(&pVal->promulgator);
	AtlCleanupValue(&pVal->title);
	AtlCleanupValue(&pVal->content);
	AtlCleanupValue(&pVal->taskState);
	AtlCleanupValue(&pVal->dispatchDate);
	AtlCleanupValue(&pVal->updateDate);
	AtlCleanupArray(pVal->relativeFileArr, pVal->__relativeFileArr_nSizeIs);
	AtlCleanupValue(&pVal->reply);
	AtlCleanupValue(&pVal->notes);
	AtlCleanupValue(&pVal->remark);
	AtlCleanupValue(&pVal->remarkVisible);
	AtlCleanupValue(&pVal->isGroup);
	AtlCleanupValue(&pVal->groupId);
	AtlCleanupValue(&pVal->categoryId);
	AtlCleanupValue(&pVal->tag);
	AtlCleanupValue(&pVal->serialNo);
	AtlCleanupValue(&pVal->productFlag);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::TaskDispatch>(XMServer::TaskDispatch *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->acceptUser, pMemMgr);
	AtlCleanupValueEx(&pVal->promulgator, pMemMgr);
	AtlCleanupValueEx(&pVal->title, pMemMgr);
	AtlCleanupValueEx(&pVal->content, pMemMgr);
	AtlCleanupValueEx(&pVal->taskState, pMemMgr);
	AtlCleanupValueEx(&pVal->dispatchDate, pMemMgr);
	AtlCleanupValueEx(&pVal->updateDate, pMemMgr);
	if (pVal->relativeFileArr != NULL)
	{
		AtlCleanupArrayEx(pVal->relativeFileArr, pVal->__relativeFileArr_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->relativeFileArr);
	}
	AtlCleanupValueEx(&pVal->reply, pMemMgr);
	AtlCleanupValueEx(&pVal->notes, pMemMgr);
	AtlCleanupValueEx(&pVal->remark, pMemMgr);
	AtlCleanupValueEx(&pVal->remarkVisible, pMemMgr);
	AtlCleanupValueEx(&pVal->isGroup, pMemMgr);
	AtlCleanupValueEx(&pVal->groupId, pMemMgr);
	AtlCleanupValueEx(&pVal->categoryId, pMemMgr);
	AtlCleanupValueEx(&pVal->tag, pMemMgr);
	AtlCleanupValueEx(&pVal->serialNo, pMemMgr);
	AtlCleanupValueEx(&pVal->productFlag, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::Role>(XMServer::Role *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->roleName);
	AtlCleanupValue(&pVal->funcOperPermBytes);
	AtlCleanupValue(&pVal->notes);
	AtlCleanupValue(&pVal->tag);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::Role>(XMServer::Role *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->roleName, pMemMgr);
	AtlCleanupValueEx(&pVal->funcOperPermBytes, pMemMgr);
	AtlCleanupValueEx(&pVal->notes, pMemMgr);
	AtlCleanupValueEx(&pVal->tag, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::Production>(XMServer::Production *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->userId);
	AtlCleanupValue(&pVal->name);
	AtlCleanupValue(&pVal->notes);
	AtlCleanupValue(&pVal->Tag);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::Production>(XMServer::Production *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->userId, pMemMgr);
	AtlCleanupValueEx(&pVal->name, pMemMgr);
	AtlCleanupValueEx(&pVal->notes, pMemMgr);
	AtlCleanupValueEx(&pVal->Tag, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::User>(XMServer::User *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->client);
	AtlCleanupValue(&pVal->userName);
	AtlCleanupValue(&pVal->password);
	AtlCleanupValue(&pVal->reallyName);
	AtlCleanupValue(&pVal->sex);
	AtlCleanupValue(&pVal->rank);
	AtlCleanupValue(&pVal->email);
	AtlCleanupValue(&pVal->nowTask);
	AtlCleanupValue(&pVal->taskSubmitDate);
	AtlCleanupValue(&pVal->enrolTime);
	AtlCleanupValue(&pVal->dWord);
	AtlCleanupValue(&pVal->notes);
	AtlCleanupArray(pVal->roleArr, pVal->__roleArr_nSizeIs);
	AtlCleanupArray(pVal->productArr, pVal->__productArr_nSizeIs);
	AtlCleanupValue(&pVal->tag);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::User>(XMServer::User *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->client, pMemMgr);
	AtlCleanupValueEx(&pVal->userName, pMemMgr);
	AtlCleanupValueEx(&pVal->password, pMemMgr);
	AtlCleanupValueEx(&pVal->reallyName, pMemMgr);
	AtlCleanupValueEx(&pVal->sex, pMemMgr);
	AtlCleanupValueEx(&pVal->rank, pMemMgr);
	AtlCleanupValueEx(&pVal->email, pMemMgr);
	AtlCleanupValueEx(&pVal->nowTask, pMemMgr);
	AtlCleanupValueEx(&pVal->taskSubmitDate, pMemMgr);
	AtlCleanupValueEx(&pVal->enrolTime, pMemMgr);
	AtlCleanupValueEx(&pVal->dWord, pMemMgr);
	AtlCleanupValueEx(&pVal->notes, pMemMgr);
	if (pVal->roleArr != NULL)
	{
		AtlCleanupArrayEx(pVal->roleArr, pVal->__roleArr_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->roleArr);
	}
	if (pVal->productArr != NULL)
	{
		AtlCleanupArrayEx(pVal->productArr, pVal->__productArr_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->productArr);
	}
	AtlCleanupValueEx(&pVal->tag, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::Client>(XMServer::Client *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->unitName);
	AtlCleanupValue(&pVal->address);
	AtlCleanupValue(&pVal->linkman);
	AtlCleanupValue(&pVal->telephone);
	AtlCleanupValue(&pVal->fax);
	AtlCleanupValue(&pVal->province);
	AtlCleanupValue(&pVal->postalCode);
	AtlCleanupValue(&pVal->enrollTime);
	AtlCleanupArray(pVal->productionArr, pVal->__productionArr_nSizeIs);
	AtlCleanupArray(pVal->downloadItemArr, pVal->__downloadItemArr_nSizeIs);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::Client>(XMServer::Client *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->unitName, pMemMgr);
	AtlCleanupValueEx(&pVal->address, pMemMgr);
	AtlCleanupValueEx(&pVal->linkman, pMemMgr);
	AtlCleanupValueEx(&pVal->telephone, pMemMgr);
	AtlCleanupValueEx(&pVal->fax, pMemMgr);
	AtlCleanupValueEx(&pVal->province, pMemMgr);
	AtlCleanupValueEx(&pVal->postalCode, pMemMgr);
	AtlCleanupValueEx(&pVal->enrollTime, pMemMgr);
	if (pVal->productionArr != NULL)
	{
		AtlCleanupArrayEx(pVal->productionArr, pVal->__productionArr_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->productionArr);
	}
	if (pVal->downloadItemArr != NULL)
	{
		AtlCleanupArrayEx(pVal->downloadItemArr, pVal->__downloadItemArr_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->downloadItemArr);
	}
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::LoginClass>(XMServer::LoginClass *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->sessionId);
	AtlCleanupValue(&pVal->user);
	AtlCleanupValue(&pVal->tag);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::LoginClass>(XMServer::LoginClass *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->sessionId, pMemMgr);
	AtlCleanupValueEx(&pVal->user, pMemMgr);
	AtlCleanupValueEx(&pVal->tag, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::PartBuild>(XMServer::PartBuild *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->name);
	AtlCleanupValue(&pVal->place);
	AtlCleanupValue(&pVal->partItemId);
	AtlCleanupValue(&pVal->submitDate);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::PartBuild>(XMServer::PartBuild *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->name, pMemMgr);
	AtlCleanupValueEx(&pVal->place, pMemMgr);
	AtlCleanupValueEx(&pVal->partItemId, pMemMgr);
	AtlCleanupValueEx(&pVal->submitDate, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::TestItemSet>(XMServer::TestItemSet *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->title);
	AtlCleanupValue(&pVal->proByte);
	AtlCleanupValue(&pVal->categoryId);
	AtlCleanupValue(&pVal->submit_date);
	AtlCleanupValue(&pVal->notes);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::TestItemSet>(XMServer::TestItemSet *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->title, pMemMgr);
	AtlCleanupValueEx(&pVal->proByte, pMemMgr);
	AtlCleanupValueEx(&pVal->categoryId, pMemMgr);
	AtlCleanupValueEx(&pVal->submit_date, pMemMgr);
	AtlCleanupValueEx(&pVal->notes, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::Category>(XMServer::Category *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->objType);
	AtlCleanupValue(&pVal->name);
	AtlCleanupValue(&pVal->parentId);
	AtlCleanupValue(&pVal->proByte);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::Category>(XMServer::Category *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->objType, pMemMgr);
	AtlCleanupValueEx(&pVal->name, pMemMgr);
	AtlCleanupValueEx(&pVal->parentId, pMemMgr);
	AtlCleanupValueEx(&pVal->proByte, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::Client_Production>(XMServer::Client_Production *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->client_id);
	AtlCleanupValue(&pVal->production_id);
	AtlCleanupValue(&pVal->tag);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::Client_Production>(XMServer::Client_Production *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->client_id, pMemMgr);
	AtlCleanupValueEx(&pVal->production_id, pMemMgr);
	AtlCleanupValueEx(&pVal->tag, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::XMSession>(XMServer::XMSession *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->sessionId);
	AtlCleanupArray(pVal->RoleArr, pVal->__RoleArr_nSizeIs);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::XMSession>(XMServer::XMSession *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->sessionId, pMemMgr);
	if (pVal->RoleArr != NULL)
	{
		AtlCleanupArrayEx(pVal->RoleArr, pVal->__RoleArr_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->RoleArr);
	}
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::LoginStruct>(XMServer::LoginStruct *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->sessionId);
	AtlCleanupValue(&pVal->user);
	AtlCleanupValue(&pVal->XMS);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::LoginStruct>(XMServer::LoginStruct *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->sessionId, pMemMgr);
	AtlCleanupValueEx(&pVal->user, pMemMgr);
	AtlCleanupValueEx(&pVal->XMS, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::WorkLog>(XMServer::WorkLog *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->hasTestItem);
	AtlCleanupValue(&pVal->description);
	AtlCleanupValue(&pVal->submitDate);
	AtlCleanupValue(&pVal->user);
	AtlCleanupValue(&pVal->title);
	AtlCleanupValue(&pVal->notes);
	AtlCleanupValue(&pVal->startDate);
	AtlCleanupValue(&pVal->endDate);
	AtlCleanupArray(pVal->testItemArr, pVal->__testItemArr_nSizeIs);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::WorkLog>(XMServer::WorkLog *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->hasTestItem, pMemMgr);
	AtlCleanupValueEx(&pVal->description, pMemMgr);
	AtlCleanupValueEx(&pVal->submitDate, pMemMgr);
	AtlCleanupValueEx(&pVal->user, pMemMgr);
	AtlCleanupValueEx(&pVal->title, pMemMgr);
	AtlCleanupValueEx(&pVal->notes, pMemMgr);
	AtlCleanupValueEx(&pVal->startDate, pMemMgr);
	AtlCleanupValueEx(&pVal->endDate, pMemMgr);
	if (pVal->testItemArr != NULL)
	{
		AtlCleanupArrayEx(pVal->testItemArr, pVal->__testItemArr_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->testItemArr);
	}
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::Version_testItem>(XMServer::Version_testItem *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->version);
	AtlCleanupValue(&pVal->testItem);
	AtlCleanupValue(&pVal->processed);
	AtlCleanupValue(&pVal->notes);
	AtlCleanupValue(&pVal->result);
	AtlCleanupValue(&pVal->user);
	AtlCleanupValue(&pVal->update_date);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::Version_testItem>(XMServer::Version_testItem *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->version, pMemMgr);
	AtlCleanupValueEx(&pVal->testItem, pMemMgr);
	AtlCleanupValueEx(&pVal->processed, pMemMgr);
	AtlCleanupValueEx(&pVal->notes, pMemMgr);
	AtlCleanupValueEx(&pVal->result, pMemMgr);
	AtlCleanupValueEx(&pVal->user, pMemMgr);
	AtlCleanupValueEx(&pVal->update_date, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::Job>(XMServer::Job *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->title);
	AtlCleanupValue(&pVal->content);
	AtlCleanupValue(&pVal->promulgator);
	AtlCleanupValue(&pVal->date);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::Job>(XMServer::Job *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->title, pMemMgr);
	AtlCleanupValueEx(&pVal->content, pMemMgr);
	AtlCleanupValueEx(&pVal->promulgator, pMemMgr);
	AtlCleanupValueEx(&pVal->date, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::VersionRevision>(XMServer::VersionRevision *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->version);
	AtlCleanupValue(&pVal->product);
	AtlCleanupValue(&pVal->promulgateDate);
	AtlCleanupValue(&pVal->promulgator);
	AtlCleanupArray(pVal->relativeFileArr, pVal->__relativeFileArr_nSizeIs);
	AtlCleanupValue(&pVal->descriptionInner);
	AtlCleanupValue(&pVal->descriptionOut);
	AtlCleanupValue(&pVal->tag);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::VersionRevision>(XMServer::VersionRevision *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->version, pMemMgr);
	AtlCleanupValueEx(&pVal->product, pMemMgr);
	AtlCleanupValueEx(&pVal->promulgateDate, pMemMgr);
	AtlCleanupValueEx(&pVal->promulgator, pMemMgr);
	if (pVal->relativeFileArr != NULL)
	{
		AtlCleanupArrayEx(pVal->relativeFileArr, pVal->__relativeFileArr_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->relativeFileArr);
	}
	AtlCleanupValueEx(&pVal->descriptionInner, pMemMgr);
	AtlCleanupValueEx(&pVal->descriptionOut, pMemMgr);
	AtlCleanupValueEx(&pVal->tag, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::News>(XMServer::News *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->title);
	AtlCleanupValue(&pVal->content);
	AtlCleanupValue(&pVal->issueDate);
	AtlCleanupValue(&pVal->promulgator);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::News>(XMServer::News *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->title, pMemMgr);
	AtlCleanupValueEx(&pVal->content, pMemMgr);
	AtlCleanupValueEx(&pVal->issueDate, pMemMgr);
	AtlCleanupValueEx(&pVal->promulgator, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::Notification>(XMServer::Notification *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->receiverFlagWord);
	AtlCleanupValue(&pVal->title);
	AtlCleanupValue(&pVal->content);
	AtlCleanupValue(&pVal->promulgateDate);
	AtlCleanupValue(&pVal->promulgator);
	AtlCleanupArray(pVal->relativeFileArr, pVal->__relativeFileArr_nSizeIs);
	AtlCleanupValue(&pVal->notes);
	AtlCleanupValue(&pVal->tag);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::Notification>(XMServer::Notification *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->receiverFlagWord, pMemMgr);
	AtlCleanupValueEx(&pVal->title, pMemMgr);
	AtlCleanupValueEx(&pVal->content, pMemMgr);
	AtlCleanupValueEx(&pVal->promulgateDate, pMemMgr);
	AtlCleanupValueEx(&pVal->promulgator, pMemMgr);
	if (pVal->relativeFileArr != NULL)
	{
		AtlCleanupArrayEx(pVal->relativeFileArr, pVal->__relativeFileArr_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->relativeFileArr);
	}
	AtlCleanupValueEx(&pVal->notes, pMemMgr);
	AtlCleanupValueEx(&pVal->tag, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::ProblemGrade>(XMServer::ProblemGrade *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->gradeName);
	AtlCleanupValue(&pVal->notes);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::ProblemGrade>(XMServer::ProblemGrade *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->gradeName, pMemMgr);
	AtlCleanupValueEx(&pVal->notes, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<XMServer::FeedBack>(XMServer::FeedBack *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->title);
	AtlCleanupValue(&pVal->version);
	AtlCleanupValue(&pVal->content);
	AtlCleanupValue(&pVal->scanNum);
	AtlCleanupValue(&pVal->production);
	AtlCleanupValue(&pVal->problemGrade);
	AtlCleanupValue(&pVal->proposer);
	AtlCleanupValue(&pVal->submitDate);
	AtlCleanupValue(&pVal->updateDate);
	AtlCleanupValue(&pVal->isEssential);
	AtlCleanupValue(&pVal->group);
	AtlCleanupValue(&pVal->superiorId);
	AtlCleanupValue(&pVal->state);
	AtlCleanupValue(&pVal->problemtype);
	AtlCleanupArray(pVal->relativeFileArr, pVal->__relativeFileArr_nSizeIs);
	AtlCleanupArray(pVal->replyArr, pVal->__replyArr_nSizeIs);
	AtlCleanupValue(&pVal->serialNo);
	AtlCleanupValue(&pVal->tag);
	AtlCleanupValue(&pVal->shadowSuperiorId);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<XMServer::FeedBack>(XMServer::FeedBack *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->title, pMemMgr);
	AtlCleanupValueEx(&pVal->version, pMemMgr);
	AtlCleanupValueEx(&pVal->content, pMemMgr);
	AtlCleanupValueEx(&pVal->scanNum, pMemMgr);
	AtlCleanupValueEx(&pVal->production, pMemMgr);
	AtlCleanupValueEx(&pVal->problemGrade, pMemMgr);
	AtlCleanupValueEx(&pVal->proposer, pMemMgr);
	AtlCleanupValueEx(&pVal->submitDate, pMemMgr);
	AtlCleanupValueEx(&pVal->updateDate, pMemMgr);
	AtlCleanupValueEx(&pVal->isEssential, pMemMgr);
	AtlCleanupValueEx(&pVal->group, pMemMgr);
	AtlCleanupValueEx(&pVal->superiorId, pMemMgr);
	AtlCleanupValueEx(&pVal->state, pMemMgr);
	AtlCleanupValueEx(&pVal->problemtype, pMemMgr);
	if (pVal->relativeFileArr != NULL)
	{
		AtlCleanupArrayEx(pVal->relativeFileArr, pVal->__relativeFileArr_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->relativeFileArr);
	}
	if (pVal->replyArr != NULL)
	{
		AtlCleanupArrayEx(pVal->replyArr, pVal->__replyArr_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->replyArr);
	}
	AtlCleanupValueEx(&pVal->serialNo, pMemMgr);
	AtlCleanupValueEx(&pVal->tag, pMemMgr);
	AtlCleanupValueEx(&pVal->shadowSuperiorId, pMemMgr);
	return S_OK;
}
