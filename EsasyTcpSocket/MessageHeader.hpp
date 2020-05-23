enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};


struct DataHeader
{

	DataHeader()
	{
		dataLenght = sizeof(DataHeader);
		cmd = CMD_ERROR;
	}

	short dataLenght;
	short cmd;
};

struct Login : public DataHeader
{
	Login()
	{
		dataLenght = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char password[32];

};

struct LoginResult : public DataHeader
{
	LoginResult()
	{
		dataLenght = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};

struct NewUserJoin : public DataHeader
{
	int socket;
	NewUserJoin()
	{
		dataLenght = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		result = 0;
		socket = 0;
	}
	int result;
};

struct LogOut : public DataHeader
{
	LogOut()
	{
		dataLenght = sizeof(LogOut);
		cmd = CMD_LOGOUT;
	}
	char usrName[32];
};

struct LogOutResult : public DataHeader
{
	LogOutResult()
	{
		dataLenght = sizeof(LogOutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};