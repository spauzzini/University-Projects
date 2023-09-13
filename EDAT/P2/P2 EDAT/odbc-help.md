## Linux installation
---
sudo apt-get install unixodbc-bin unixodbc unixodbc-dev

sudo apt-get install odbc-postgresql

; you can use templates for .odbc.ini with
sudo odbcinst -i -d -f /usr/share/psqlodbc/odbcinst.ini.template

odbcinst -j # print odbc current configuration

odbcinst -q -d # query about the installed drivers

odbcinst -q -s # query about existing data sources

ODBCCreateDataSourceQ4 to create a data source

ODBCManageDataSourceQ4 to manage data sources

Use Potgresql ANSI as driver, localhost as host, and enter database, user and password fields.

`>isql -v my_dsn user database` to test the connection (requires unixodbc, not installed in labs)


## ODBC
---

### Docs

* [User Manual](http://www.unixodbc.org/doc/UserManual/) from unixodbc.org. Includes information about tools such as ODBCConfig to create datasources (but no mention of ODBCCreateDataSourcesQt or ODBCManageDataSources) and DataManager and isql to interact graphically or from the command line with your datasources.
*  [Easysoft tutorials on using ODBC from C](https://www.easysoft.com/developer/languages/c/index.html) including:
    * [ODBC from C Tutorial Part 1](https://www.easysoft.com/developer/languages/c/odbc_tutorial.html)
    * [ODBC from C Tutorial Part 2 - Fetching Results](https://www.easysoft.com/developer/languages/c/odbc-tutorial-fetching-results.html)
    * [ODBC Code Samples](https://www.easysoft.com/developer/languages/c/examples/index.html)


### ODBC Handles
In ODBC there are four main handle types and you will need to know at least three to do anything useful:

SQLHENV - environment handle
This is the first handle you will need as everything else is effectively in the environment. Once you have an environment handle you can define the version of ODBC you require, enable connection pooling and allocate connection handles with SQLSetEnvAttr and SQLAllocHandle.

SQLHDBC - **connection handle**.
You need one connection handle for each data source you are going to connect to. Like environment handles, connection handles have attributes which you can retrieve and set with SQLSetConnectAttr and SQLGetConnectAttr.

SQLHSTMT - **statement handle**.
Once you have a connection handle and have connected to a data source you allocate statement handles to execute SQL or retrieve meta data. As with the other handles you can set and get statement attributes with SQLSetStmtAttr and SQLGetStmtAttr.

SQLHDESC - **descriptor handle**
Descriptor handles are rarely used by applications even though they are very useful for more complex operations. 

Ignoring descriptors, the following code is usually in every ODBC application:
``` 
SQLHENV env;
SQLHDBC dbc;
SQLHSTMT stmt;

/* allocate environment handle */
SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);

/* ensure this environment supports ODBC 3  support */
SQLSetEnvAttr(*env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);

/* allocate connection handle in this environment */
SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);

/* connect to the data source */
SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

/* do something with the statement handle e.g. issue sql */
SQLFreeHandle(SQL_HANDLE_STMT, stmt);

/* disconnect and free resources */
SQLDisconnect(dbc);
SQLFreeHandle(SQL_HANDLE_DBC, dbc);
SQLFreeHandle(SQL_HANDLE_ENV, env);
```

### ODBC Types

| SQL | C |
|----|---|
| SQLCHAR | unsigned char |
| SQLINTEGER |long int |
| SQLSMALLINT | small int |
| SQLREAL | float |
| SQLDOUBLE | double |
| SQLPOINTER | void* |



`` 

