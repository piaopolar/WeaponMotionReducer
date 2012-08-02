#pragma warning(disable : 4786)
#include <CSTDIO>
#include <VECTOR>
#include <MAP>
#include <STRING>

const int _MAX_STRING = 1024;

const char *pszIniReduced = "WeaponMotionReduced.ini";

// ============================================================================
// ==============================================================================

void ToolMsg(const char *pszFormat, ...)
{
	if (!pszFormat) {
		return;
	}

	//~~~~~~~~~~~~~~~~~~~~~
	char buffer[_MAX_STRING];
	//~~~~~~~~~~~~~~~~~~~~~

	_vsnprintf(buffer, sizeof(buffer), pszFormat, (char *)((&pszFormat) + 1));
	printf("%s\n", buffer);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen("Process.log", "a+");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (pFile) {
		fprintf(pFile, "%s\n", buffer);
		fclose(pFile);
	}
}

struct NODE
{
	__int64 i64Index;
	std::string strIndex;
	std::string strRes;
};

// ============================================================================
// ==============================================================================
bool ReadIndexFile(const char *pszWeaponMotion,
				   std::vector<NODE> &rVecOrder,
				   std::map<__int64, std::string> &mapOrgInfo)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(pszWeaponMotion, "r");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		printf("open %s failed\n", pszWeaponMotion);
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~
	char szTmp[_MAX_STRING];
	NODE node;
	//~~~~~~~~~~~~~~~~~~~~

	while (fgets(szTmp, sizeof(szTmp), pFile)) {
		if (szTmp[0] == 0 || szTmp[0] == ';' || szTmp[0] == 10 && szTmp[1] == 0) {
			continue;
		}

		//~~~~~~~~~~~~~~~~~~~~~~
		__int64 i64Index = 0;
		char szRes[_MAX_STRING];
		char szIndex[_MAX_STRING];
		//~~~~~~~~~~~~~~~~~~~~~~

		if (2 == sscanf(szTmp, "%I64d=%s", &i64Index, szRes)) {
			mapOrgInfo[i64Index] = szRes;
			node.i64Index = i64Index;
			node.strRes = szRes;
			sscanf(szTmp, "%[^=]", szIndex);
			node.strIndex = szIndex;
			rVecOrder.push_back(node);
		} else {
			printf("error line [%s]\n", szTmp);
		}
	}

	fclose(pFile);

	printf("Org File Index size %d\n", mapOrgInfo.size());
	return true;
}

// ============================================================================
// ==============================================================================
int main()
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	char szTmp[_MAX_STRING];
	std::map<__int64, std::string> mapOrgInfo;
	std::vector<NODE> vecOrder;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	printf("WeaponMotion Reducer\n");
	printf("Drag a weaponMotion.ini file into this window:\n");
	gets(szTmp);
	ReadIndexFile(szTmp, vecOrder, mapOrgInfo);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nNewCount = 0;
	FILE *pFileOut = fopen(pszIniReduced, "w");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFileOut) {
		ToolMsg("Create %s Failed", pszIniReduced);
		return 0;
	}

	std::vector<NODE>::const_iterator it(vecOrder.begin());
	for (; it != vecOrder.end(); ++it) {

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		__int64 i64Index = it->i64Index;
		const __int64 WEAPON_MOTION_MOUNT_MUTI = 100000000000000;
		__int64 i64Mount = i64Index / WEAPON_MOTION_MOUNT_MUTI;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if (i64Mount > 1) {
			i64Index -= (i64Mount - 1) * WEAPON_MOTION_MOUNT_MUTI;
			if (mapOrgInfo[i64Index] != it->strRes) {
				ToolMsg("Error Index %I64d Res %s  !=  Reduce Index %I64d Res %s", it->i64Index, it->strRes.c_str(),
						i64Index, mapOrgInfo[i64Index].c_str());
				fprintf(pFileOut, "%s=%s\n", it->strIndex.c_str(), it->strRes.c_str());
				++nNewCount;
			}

			continue;
		}

		if (i64Mount) {
			i64Index -= WEAPON_MOTION_MOUNT_MUTI;
			if (mapOrgInfo[i64Index] != it->strRes) {
				fprintf(pFileOut, "%s=%s\n", it->strIndex.c_str(), it->strRes.c_str());
				++nNewCount;
			}

			continue;
		}

		fprintf(pFileOut, "%s=%s\n", it->strIndex.c_str(), it->strRes.c_str());

		++nNewCount;
	}

	fclose(pFileOut);
	ToolMsg("Done. %d Index Save to %s", nNewCount, pszIniReduced);

	while (getchar());

	return 0;
}
