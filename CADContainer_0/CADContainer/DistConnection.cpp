#include "distconnection.h"

//================================ export function ======================================//

char* WINAPI ReturnErrorCodeInfo(long nErrorCode)
{
	static char strInfo[SE_MAX_MESSAGE_LENGTH ] ={0};
	memset(strInfo,0,sizeof(char)*SE_MAX_MESSAGE_LENGTH);
	switch(nErrorCode)
	{
	    case  0:strcpy(strInfo,"\n Success!");break;
		case  1:strcpy(strInfo,"\n Success!");break;
		case -1:strcpy(strInfo,"\n failure!");break;
		case -2:strcpy(strInfo,"\n LAYERINFO pointer not initialized!");break;
		case -3:strcpy(strInfo,"\n The given shape has no annotation!");break;
		case -4:strcpy(strInfo,"\n STREAM LOADING OF SHAPES FINISHED!");break;
		case -5:strcpy(strInfo,"\n SDE NOT STARTED, CANNOT PERFORM FUNCTION !");break;
		case -6:strcpy(strInfo,"\n THE SPECIFIED SHAPE WAS LEFT UNCHANGED!");break;
		case -7:strcpy(strInfo,"\n THE NUMBER OF SERVER TASKS/CONNECTIONS IS @ MAXIMUM !");break;
		case -8:strcpy(strInfo,"\n IOMGR NOT ACCEPTING CONNECTION REQUESTS!");break;
		case -9:strcpy(strInfo,"\n CANNOT VALIDATE THE SPECIFIED USER AND PASSWORD");break;
		case -10:strcpy(strInfo,"\n NETWORK I/O OPERATION FAILED!");break;
		case -11:strcpy(strInfo,"\n NETWORK I/O TIMEOUT!");break;
		case -12:strcpy(strInfo,"\n SERVER TASK CANNOT ALLOCATE NEEDED MEMORY!");break;
		case -13  :strcpy(strInfo,"\n CLIENT TASK CANNOT ALLOCATE NEEDED MEMORY !");break;
		case -14  :strcpy(strInfo,"\n FUNCTION CALL IS OUT OF CONTEXT !");break;
		case -15  :strcpy(strInfo,"\n NO ACCESS TO OBJECT !");break;
		case -16  :strcpy(strInfo,"\n Exceeded max_layers in giomgr.defs. !");break;
		case -17  :strcpy(strInfo,"\n MISSING LAYER SPECIFICATION !");break;
		case -18  :strcpy(strInfo,"\n SPECIFIED LAYER IS LOCKED !");break;
		case -19  :strcpy(strInfo,"\n SPECIFIED LAYER ALREADY EXISTS !");break;
		case -20  :strcpy(strInfo,"\n SPECIFIED LAYER DOES NOT EXIST !");break;
		case -21  :strcpy(strInfo,"\n SPECIFIED LAYER IS USE BY ANOTHER USER !");break;
		case -22  :strcpy(strInfo,"\n SPECIFIED SHAPE (LAYER:FID) DOESN'T EXIST OR SPECIFIED ROW DOESN'T EXIST!");break;
		case -23  :strcpy(strInfo,"\n SPECIFIED SHAPE (LAYER:FID) EXISTS  OR  SPECIFIED ROW EXISTS!");break;
		case -24  :strcpy(strInfo,"\n Both layers must be the same for this !");break;
		case -25  :strcpy(strInfo,"\n NO PERMISSION TO PERFORM OPERATION !");break;
		case -26  :strcpy(strInfo,"\n COLUMN HAS NOT NULL CONSTRAINT. !");break;
		case -27  :strcpy(strInfo,"\n INVALID SHAPE, CANNOT BE VERIFIED !");break;
		case -28  :strcpy(strInfo,"\n MAP LAYER NUMBER OUT OF RANGE !");break;
		case -29  :strcpy(strInfo,"\n INVALID ENTITY TYPE !");break;
		case -30  :strcpy(strInfo,"\n INVALID SEARCH METHOD !");break;
		case -31  :strcpy(strInfo,"\n INVALID ENTITY TYPE MASK !");break;
		case -32  :strcpy(strInfo,"\n BIND/SET/GET MIS-MATCH !");break;
		case -33  :strcpy(strInfo,"\n INVALID GRID SIZE !");break;
		case -34  :strcpy(strInfo,"\n INVALID LOCK MODE !");break;
		case -35  :strcpy(strInfo,"\n ENTITY TYPE OF SHAPE IS NOT ALLOWED IN LAYER !");break;
		case -36  :strcpy(strInfo,"\n Exceeded max points specified. or Alternate name of above !");break;
		case -37  :strcpy(strInfo,"\n DBMS TABLE DOES NOT EXIST !");break;
		case -38  :strcpy(strInfo,"\n SPECIFIED ATTRIBUTE COLUMN DOESN'T EXIST !");break;
		case -39  :strcpy(strInfo,"\n Underlying license manager problem. !");break;
		case -40  :strcpy(strInfo,"\n No more SDE licenses available. !");break;
		case -41  :strcpy(strInfo,"\n VALUE EXCEEDS VALID RANGE !");break;
		case -42  :strcpy(strInfo,"\n USER SPECIFIED WHERE CLAUSE IS INVALID  OR  USER SPECIFIED SQL CLAUSE IS INVALID !");break;
		case -43  :strcpy(strInfo,"\n SPECIFIED LOG FILE DOES NOT EXIST !");break;
		case -44  :strcpy(strInfo,"\n UNABLE TO ACCESS SPECIFIED LOGFILE !");break;
		case -45  :strcpy(strInfo,"\n SPECIFIED LOGFILE IS NOT OPEN FOR I/O !");break;
		case -46  :strcpy(strInfo,"\n I/O ERROR USING LOGFILE !");break;
		case -47  :strcpy(strInfo,"\n NO SHAPES SELECTED OR USED IN OPERATION !");break;
		case -48  :strcpy(strInfo,"\n NO LOCKS DEFINED !");break;
		case -49  :strcpy(strInfo,"\n LOCK REQUEST CONFLICTS W/ ANOTHER ESTABLISHED LOCK !");break; 
		case -50  :strcpy(strInfo,"\n MAXIMUM LOCKS ALLOWED BY SYSTEM ARE IN USE !");break;
		case -51  :strcpy(strInfo,"\n DATABASE LEVEL I/O ERROR OCCURRED !");break;
		case -52  :strcpy(strInfo,"\n SHAPE/FID STREAM NOT FINISHED, CAN'T EXECUTE !");break;
		case -53  :strcpy(strInfo,"\n INVALID COLUMN DATA TYPE !");break;
		case -54  :strcpy(strInfo,"\n TOPOLOGICAL INTEGRITY ERROR !");break;
		case -55  :strcpy(strInfo,"\n ATTRIBUTE CONVERSION ERROR !");break;
		case -56  :strcpy(strInfo,"\n INVALID COLUMN DEFINITION !");break;
		case -57  :strcpy(strInfo,"\n INVALID SHAPE ARRAY BUFFER SIZE !");break;
		case -58  :strcpy(strInfo,"\n ENVELOPE IS NULL, HAS NEGATIVE VALUES OR MIN > MAX !");break;
		case -59  :strcpy(strInfo,"\n TEMP FILE I/O ERROR, CAN'T OPEN OR RAN OUT OF DISK !");break;
		case -60  :strcpy(strInfo,"\n SPATIAL INDEX GRID SIZE IS TOO SMALL !");break;
		case -61  :strcpy(strInfo,"\n SDE RUN-TIME LICENSE HAS EXPIRED, NO LOGINS ALLOWED !");break;
		case -62  :strcpy(strInfo,"\n DBMS TABLE EXISTS !");break;
		case -63  :strcpy(strInfo,"\n INDEX WITH THE SPECIFIED NAME ALREADY EXISTS !");break;
		case -64  :strcpy(strInfo,"\n INDEX WITH THE SPECIFIED NAME DOESN'T EXIST !");break;
		case -65  :strcpy(strInfo,"\n SPECIFIED POINTER VALUE IS NULL OR INVALID !");break;
		case -66  :strcpy(strInfo,"\n SPECIFIED PARAMETER VALUE IS INVALID !");break;
		case -67  :strcpy(strInfo,"\n SLIVER FACTOR CAUSED ALL RESULTS TO BE SLIVERS !");break;
		case -68  :strcpy(strInfo,"\n USER SPECIFIED TRANSACTION IN PROGRESS !");break;
		case -69  :strcpy(strInfo,"\n The iomgr has lost its connection to the underlying DBMS. !");break;
		case -70  :strcpy(strInfo,"\n AN ARC (startpt,midpt,endpt) ALREADY EXISTS !");break;
		case -71  :strcpy(strInfo,"\n SE_ANNO pointer not initialized. !");break;
		case -72  :strcpy(strInfo,"\n SPECIFIED POINT DOESN'T EXIST IN FEAT !");break;
		case -73  :strcpy(strInfo,"\n SPECIFIED POINTS MUST BE ADJACENT !");break;
		case -74  :strcpy(strInfo,"\n SPECIFIED MID POINT IS INVALID !");break;
		case -75  :strcpy(strInfo,"\n SPECIFIED END POINT IS INVALID !");break;
		case -76  :strcpy(strInfo,"\n SPECIFIED RADIUS IS INVALID !");break;
		case -77  :strcpy(strInfo,"\n MAP LAYER IS LOAD ONLY MODE, OPERATION NOT ALLOWED !");break;
		case -78  :strcpy(strInfo,"\n LAYERS TABLE DOES NOT EXIST. !");break;
		case -79  :strcpy(strInfo,"\n Error writing or creating an output text file. !");break;
		case -80  :strcpy(strInfo,"\n Maximum BLOB size exceeded. !");break;
		case -81  :strcpy(strInfo,"\n Resulting corridor exceeds valid coordinate range !");break;
		case -82  :strcpy(strInfo,"\n MODEL INTEGRITY ERROR !");break;
		case -83  :strcpy(strInfo,"\n Function or option is not really  written yet. !");break;
		case -84  :strcpy(strInfo,"\n This shape has a cad. !");break;
		case -85  :strcpy(strInfo,"\n Invalid internal SDE Transaction ID. !");break;
		case -86  :strcpy(strInfo,"\n MAP LAYER NAME MUST NOT BE EMPTY !");break;
		case -87  :strcpy(strInfo,"\n Invalid Layer Configuration Keyword used. !");break;
		case -88  :strcpy(strInfo,"\n Invalid Release/Version of SDE server. !");break;
		case -89  :strcpy(strInfo,"\n VERSION table exists. !");break;
		case -90  :strcpy(strInfo,"\n Column has not been bound !");break;
		case -91  :strcpy(strInfo,"\n Indicator variable contains an invalid value !");break;
		case -92   :strcpy(strInfo,"\n The connection handle is NULL, closed or the wrong object. !");break;
		case -93   :strcpy(strInfo,"\n The DBA password is not correct. !");break;
		case -94   :strcpy(strInfo,"\n Coord path not found in shape edit op. !");break;
		case -95   :strcpy(strInfo,"\n No SDEHOME environment var set, and we need one. !");break;
		case -96   :strcpy(strInfo,"\n User must be table owner. !");break;
		case -97   :strcpy(strInfo,"\n The process ID specified does not correspond on an SDE server.  !");break;
		case -98   :strcpy(strInfo,"\n DBMS didn't accept user/password. !");break;
		case -99   :strcpy(strInfo,"\n Password received was sent > MAXTIMEDIFF seconds before. !");break;
		case -100  :strcpy(strInfo,"\n Server machine was not found !");break;
		case -101  :strcpy(strInfo,"\n IO Mgr task not started on server !");break;
		case -102  :strcpy(strInfo,"\n No SDE entry in the /etc/services file !");break;
		case -103  :strcpy(strInfo,"\n Tried statisitics on non-numeric !");break;
		case -104  :strcpy(strInfo,"\n Distinct stats on invalid type !");break;
		case -105  :strcpy(strInfo,"\n Invalid use of grant/revoke function !");break;
		case -106  :strcpy(strInfo,"\n The supplied SDEHOME path is invalid or NULL.  !");break;
		case -107  :strcpy(strInfo,"\n Stream does not exist !");break;
		case -108  :strcpy(strInfo,"\n Max number of streams exceeded !");break;
		case -109  :strcpy(strInfo,"\n Exceeded system's max number of mutexs. !");break;
		case -110  :strcpy(strInfo,"\n This connection is locked to a  different thread. !");break;
		case -111  :strcpy(strInfo,"\n This connection is being used at the moment by another thread. !");break;
		case -112  :strcpy(strInfo,"\n The SQL statement was not a select !");break;
		case -113  :strcpy(strInfo,"\n Function called out of sequence !");break;
		case -114  :strcpy(strInfo,"\n Get request on wrong column type !");break;
		case -115  :strcpy(strInfo,"\n This ptable is locked to a different thread. !");break;
		case -116  :strcpy(strInfo,"\n This ptable is being used at the moment by another thread. !");break;
		case -117  :strcpy(strInfo,"\n This stable is locked to a different thread. !");break;
		case -118  :strcpy(strInfo,"\n This stable is being used at the moment by another thread. !");break;
		case -119  :strcpy(strInfo,"\n Unrecognized spatial filter type. !");break;
		case -120  :strcpy(strInfo,"\n The given shape has no CAD. !");break;
		case -121  :strcpy(strInfo,"\n No instance running on server. !");break;
		case -122  :strcpy(strInfo,"\n Instance is a version previous to 2.0. !");break;
		case -123  :strcpy(strInfo,"\n Systems units < 1 or > 2147483647. !");break;
		case -124  :strcpy(strInfo,"\n FEET, METERS, DECIMAL_DEGREES or OTHER. !");break;
		case -125  :strcpy(strInfo,"\n SE_CAD pointer not initialized. !");break;
		case -126  :strcpy(strInfo,"\n Version not found. !");break;
		case -127  :strcpy(strInfo,"\n Spatial filters invalid for search !");break;
		case -128  :strcpy(strInfo,"\n Invalid operation for the given stream !");break;
		case -129  :strcpy(strInfo,"\n Column contains NOT NULL values during SE_layer_create() !");break;
		case -130  :strcpy(strInfo,"\n No spatial masks available.  !");break;
		case -131  :strcpy(strInfo,"\n Iomgr program not found. !");break;
		case -132  :strcpy(strInfo,"\n Operation can not possibly be run on this system -- it needs a server. !");break;
		case -133  :strcpy(strInfo,"\n Only one spatial column allowed !");break;
		case -134  :strcpy(strInfo,"\n The given shape object handle is invalid !");break;
		case -135  :strcpy(strInfo,"\n The specified shape part number does not exist !");break;
		case -136  :strcpy(strInfo,"\n The given shapes are of incompatible types !");break;
		case -137  :strcpy(strInfo,"\n The specified part offset is invalid !");break;
		case -138  :strcpy(strInfo,"\n The given coordinate references are incompatible !");break;
		case -139  :strcpy(strInfo,"\n The specified coordinate exceeds the valid coordinate range !");break;
		case -140  :strcpy(strInfo,"\n Max. Layers exceeded in cache !");break;
		case -141  :strcpy(strInfo,"\n The given coordinate reference  object handle is invalid !");break;
		case -142  :strcpy(strInfo,"\n The coordinate system identifier is invalid !");break;
		case -143  :strcpy(strInfo,"\n The coordinate system description is invalid !");break;
		case -144  :strcpy(strInfo,"\n SE_ROW_ID is not a valid ArcSDE row_id column for this table or layer !");break;
		case -145  :strcpy(strInfo,"\n Error projecting shape points !");break;
		case -146  :strcpy(strInfo,"\n Max array bytes exceeded !");break;
		case -147  :strcpy(strInfo,"\n 2 donuts or 2 outer shells overlap !");break;
		case -148  :strcpy(strInfo,"\n Numofpts is less than required for feature !");break;
		case -149 :strcpy(strInfo,"\n part separator in the wrong position !");break;
		case -150 :strcpy(strInfo,"\n polygon does not close properly !");break;
		case -151 :strcpy(strInfo,"\n A polygon outer shell does not completely enclose all donuts for the part !");break;
		case -152 :strcpy(strInfo,"\n Polygon shell has no area !");break;
		case -153 :strcpy(strInfo,"\n Polygon shell contains a vertical line !");break;
		case -154 :strcpy(strInfo,"\n Multipart area has overlapping parts !");break;
		case -155 :strcpy(strInfo,"\n Linestring or poly boundary is self-intersecting !");break;
		case -156 :strcpy(strInfo,"\n Export file is invalid !");break;
		case -157 :strcpy(strInfo,"\n Attempted to modify or free a read-only shape from an stable. !");break;
		case -158 :strcpy(strInfo,"\n Invalid data source !");break;
		case -159 :strcpy(strInfo,"\n Stream Spec parameter exceeds giomgr default !");break;
		case -160 :strcpy(strInfo,"\n Tried to remove cad or anno !");break;
		case -161 :strcpy(strInfo,"\n Spat col name same as table name !");break;
		case -162 :strcpy(strInfo,"\n Invalid database name !");break;
		case -163 :strcpy(strInfo,"\n Spatial SQL extension not present in underlying DBMS !");break;
		case -164 :strcpy(strInfo,"\n Obsolete SDE 3.0.2 error !");break;
		case -165 :strcpy(strInfo,"\n Obsolete SDE 3.0.2 error !");break;
		case -166 :strcpy(strInfo,"\n SDE 3.0.2 error !");break;
		case -167 :strcpy(strInfo,"\n Obsolete SDE 3.0.2 error!");break;
		case -168 :strcpy(strInfo,"\n Has R/O access to SE_ROW_ID !");break;
		case -169 :strcpy(strInfo,"\n The current table doesn't have a SDE-maintained rowid column. !");break;
		case -170 :strcpy(strInfo,"\n Column is not user-modifiable !");break;
		case -171 :strcpy(strInfo,"\n Illegal or blank version name !");break;
		case -172 :strcpy(strInfo,"\n A specified state is not in the VERSION_STATES table. !");break;
		case -173 :strcpy(strInfo,"\n STATEINFO object not initialized. !");break;
		case -174 :strcpy(strInfo,"\n Attempted to change version state, but already changed. !");break;
		case -175 :strcpy(strInfo,"\n Tried to open a state which has children. !");break;
		case -176 :strcpy(strInfo,"\n To create a state, the parent state must be closed. !");break;
		case -177 :strcpy(strInfo,"\n Version already exists. !");break;
		case -178 :strcpy(strInfo,"\n Table must be multiversion for this operation. !");break;
		case -179 :strcpy(strInfo,"\n Can't delete state being used by a version. !");break;
		case -180 :strcpy(strInfo,"\n VERSIONINFO object not  initialized. !");break;
		case -181 :strcpy(strInfo,"\n State ID out of range or not found. !");break;
		case -182 :strcpy(strInfo,"\n Environment var SDETRACELOC not set to a value   !");break;
		case -183 :strcpy(strInfo,"\n Error loading the SSA  !");break;
		case -184 :strcpy(strInfo,"\n This operation has more states than can fit in SQL. !");break;
		case -185 :strcpy(strInfo,"\n Function takes 2 <> states, but same one was given twice. !");break;
		case -186 :strcpy(strInfo,"\n Table has no usable row ID column. !");break;
		case -187 :strcpy(strInfo,"\n Call needs state to be set. !");break;
		case -188 :strcpy(strInfo,"\n Error executing SSA function !");break;
		case -189 :strcpy(strInfo,"\n REGINFO object !initialized. !");break;
		case -190 :strcpy(strInfo,"\n Attempting to trim between states on diff. branches !");break;
		case -191 :strcpy(strInfo,"\n State is being modified. !");break;
		case -192 :strcpy(strInfo,"\n Try to lock tree, and some state in tree already locked. !");break;
		case -193 :strcpy(strInfo,"\n Raster column has non-NULL values or used as row_id column !");break;
		case -194 :strcpy(strInfo,"\n Raster column already exists !");break;
		case -195 :strcpy(strInfo,"\n Unique indexes are not allowed on multiversion tables. !");break;
		case -196 :strcpy(strInfo,"\n Invalid layer storage type. !");break;
		case -197 :strcpy(strInfo,"\n No SQL type provided when  converting NIL shape to text !");break;
		case -198 :strcpy(strInfo,"\n Invalid byte order for  Well-Known Binary shape !");break;
		case -199 :strcpy(strInfo,"\n Shape type in the given shape is not a valid geometry type !");break;
		case -200 :strcpy(strInfo,"\n Number of measures in shape must be zero or equal to number  of points !");break;
		case -201 :strcpy(strInfo,"\n Number of parts in shape is incorrect for its geometry type !");break;
		case -202 :strcpy(strInfo,"\n Memory allocated for ESRI binary shape is too small !");break;
		case -203 :strcpy(strInfo,"\n Shape text exceeds the  supplied maximum string length !");break;
		case -204 :strcpy(strInfo,"\n Found syntax error in the  supplied shape text !");break;
		case -205 :strcpy(strInfo,"\n Number of parts in shape is more than expected for the given shape text !");break;
		case -206 :strcpy(strInfo,"\n Shape's SQL type is not as  expected when constructing  shape from text !");break;
		case -207 :strcpy(strInfo,"\n Found parentheses mismatch when parsing shape text !");break;
		case -208 :strcpy(strInfo,"\n NIL shape is not allowed for Well-Known Binary  represenation !");break;
		case -209 :strcpy(strInfo,"\n Tried to start already running SDE instance. !");break;
		case -210 :strcpy(strInfo,"\n The operation requested is  unsupported. !");break;
		case -211 :strcpy(strInfo,"\n Invalid External layer option. !");break; 
		case -212 :strcpy(strInfo,"\n Normalized layer dimension  table value not found. !");break;
		case -213 :strcpy(strInfo,"\n Invalid query type. !");break;
		case -214 :strcpy(strInfo,"\n No trace functions in this library !");break;
		case -215 :strcpy(strInfo,"\n Tried to enable tracing that was already on !");break; 
		case -216 :strcpy(strInfo,"\n Tried to disable tracing that was already off !");break; 
		case -217 :strcpy(strInfo,"\n SCL Compiler doesn't like the SCL stmt !");break;
		case -218 :strcpy(strInfo,"\n Table already registered. !");break;
		case -219 :strcpy(strInfo,"\n Registration ID out of range !");break;
		case -220 :strcpy(strInfo,"\n Table not registered. !");break;
		case -221 :strcpy(strInfo,"\n Exceeded max_registrations. !");break;
		case -222 :strcpy(strInfo,"\n This object can not be deleted,other objects depend on it. !");break;
		case -223 :strcpy(strInfo,"\n Row locking enabled      !");break;
		case -224 :strcpy(strInfo,"\n Row locking not enabled  !");break;
		case -225 :strcpy(strInfo,"\n Specified raster column is used by another user !");break;
		case -226 :strcpy(strInfo,"\n The specified raster column  does not exist !");break;
		case -227 :strcpy(strInfo,"\n Raster column number  out of range !");break;
		case -228 :strcpy(strInfo,"\n Maximum raster column  number exceeded !");break;
		case -229 :strcpy(strInfo,"\n Raster number out of range !");break;
		case -230 :strcpy(strInfo,"\n cannot determine  request status !");break;
		case -231 :strcpy(strInfo,"\n cannot open request results !");break; 
		case -232 :strcpy(strInfo,"\n Raster band already exists !");break;
		case -233 :strcpy(strInfo,"\n The specified raster band does not exist !");break;
		case -234 :strcpy(strInfo,"\n Raster already exists !");break;
		case -235 :strcpy(strInfo,"\n The specified raster  does not exist !");break;
		case -236 :strcpy(strInfo,"\n Maximum raster band number exceeded !");break; 
		case -237 :strcpy(strInfo,"\n Maximum raster number  exceeded !");break; 
		case -238 :strcpy(strInfo,"\n DBMS VIEW EXISTS !");break;
		case -239 :strcpy(strInfo,"\n DBMS VIEW NOT EXISTS !");break;
		case -240 :strcpy(strInfo,"\n Lock record exist !");break;
		case -241 :strcpy(strInfo,"\n Rowlocking mask conflict !");break;
		case -242 :strcpy(strInfo,"\n Raster band specified  not in a raster !");break;
		case -243 :strcpy(strInfo,"\n RASBANDINFO object not initialized !");break;
		case -244 :strcpy(strInfo,"\n RASCOLINFO object not initialized !");break;
		case -245 :strcpy(strInfo,"\n RASTERINFO object  not initialized !");break;
		case -246 :strcpy(strInfo,"\n Raster band number out of range !");break;
		case -247 :strcpy(strInfo,"\n Only one raster column allowed !");break;
		case -248 :strcpy(strInfo,"\n Table is being locked already !");break;
		case -249 :strcpy(strInfo,"\n SE_LOGINFO pointer not initialized. !");break;
		case -250 :strcpy(strInfo,"\n Operation generated a SQL statement too big to process.!");break;
		case -251 :strcpy(strInfo,"\n Not supported on a View.!");break;
		case -252 :strcpy(strInfo,"\n Specified log file exists already. !");break;
		case -253 :strcpy(strInfo,"\n Specified log file is open. !");break;
		case -254 :strcpy(strInfo,"\n Spatial reference entry exists. !");break;
		case -255 :strcpy(strInfo,"\n Spatial reference entry does not exist. !");break;
		case -256 :strcpy(strInfo,"\n Spatial reference entry is in use by one or more layers. !");break;
		case -257 :strcpy(strInfo,"\n The SE_SPATIALREFINFO object is not initialized. !");break;
		case -258 :strcpy(strInfo,"\n Raster band sequence number already exits. !");break;
		case -259 :strcpy(strInfo,"\n SE_QUERYINFO pointer not initialized. !");break;
		case -260 :strcpy(strInfo,"\n SE_QUERYINFO pointer is not prepared for query. !");break;
		case -261 :strcpy(strInfo,"\n RASTILEINFO object not initialized !");break;
		case -262 :strcpy(strInfo,"\n SE_RASCONSTRAINT object not initialized !");break;
		case -263 :strcpy(strInfo,"\n invalid record id number !");break;
		case -264 :strcpy(strInfo,"\n SE_METADATAINFO pointer not initialized !");break;
		case -265 :strcpy(strInfo,"\n unsupported object type !");break;
		case -266 :strcpy(strInfo,"\n SDEMETADATA table does not exist !");break;
		case -267 :strcpy(strInfo,"\n Metadata record does not exist. !");break;
		case -268 :strcpy(strInfo,"\n Geometry entry does not exist !");break;
		case -269 :strcpy(strInfo,"\n File path too long or invalid !");break;
		case -270 :strcpy(strInfo,"\n Locator object not initialized !");break;
		case -271 :strcpy(strInfo,"\n Locator cannot be validated !");break;
		case -272 :strcpy(strInfo,"\n Table has no associated locator !");break;
		case -273 :strcpy(strInfo,"\n Locator cateogry is not specified !");break;
		case -274 :strcpy(strInfo,"\n Invalid locator name !");break;
		case -275 :strcpy(strInfo,"\n Locator does not exist !");break;
		case -276 :strcpy(strInfo,"\n A locator with that name exists !");break;
		case -277 :strcpy(strInfo,"\n Unsupported Locator type !");break;
		case -278 :strcpy(strInfo,"\n No coordref defined !");break;
		case -279 :strcpy(strInfo,"\n Can't trim past a reconciled state. !");break;
		case -280 :strcpy(strInfo,"\n Fileinfo object does not exist. !");break;
		case -281 :strcpy(strInfo,"\n Fileinfo object already exists. !");break;
		case -282 :strcpy(strInfo,"\n Fileinfo object not initialized. !");break;
		case -283 :strcpy(strInfo,"\n Unsupported Fileinfo object type. !");break;
		case -284 :strcpy(strInfo,"\n No statistics available for this raster band. !");break;
		case -285 :strcpy(strInfo,"\n Can't delete a version with children. !");break;
		case -286 :strcpy(strInfo,"\n SQL type does not support ANNO or CAD at current release !");break;
		case -287 :strcpy(strInfo,"\n The DBTUNE file is missing  or unreadable. !");break;
		case -288 :strcpy(strInfo,"\n Logfile system tables do not exist. !");break;
		case -289 :strcpy(strInfo,"\n This app can't perform this operation on this object. !");break;
		case -290 :strcpy(strInfo,"\n The given geographic transformation object handle is invalid !");break;
		case -291 :strcpy(strInfo,"\n Column already exists !");break;
		case -292 :strcpy(strInfo,"\n SQL keyword violation. !");break;
		case -293 :strcpy(strInfo,"\n The supplied objectlock handle is bad. !");break;
		case -294 :strcpy(strInfo,"\n The raster buffer size is too small. !");break;
		case -295 :strcpy(strInfo,"\n Invalid raster data !");break;
		case -296 :strcpy(strInfo,"\n This operation is not allowed !");break;
		case -297 :strcpy(strInfo,"\n SE_RASTERATTR object not  initialized !");break;
		case -298 :strcpy(strInfo,"\n Version ID out of range. !");break;
		case -299 :strcpy(strInfo,"\n Attempting to make an MV table load-only !");break;
		case -300 :strcpy(strInfo,"\n The user-supplied table/column is invalid. !");break;
		case -301 :strcpy(strInfo,"\n The next row was not the row expected. !");break;
		case -302 :strcpy(strInfo,"\n The ArcSDE instance is  read-only !");break;
		case -303 :strcpy(strInfo,"\n Image mosaicking is not allowed !");break;
		case -304 :strcpy(strInfo,"\n Invalid raster bitmap  object !");break;
		case -305 :strcpy(strInfo,"\n The specified band sequence  number does not exist. !");break;
		case -306 :strcpy(strInfo,"\n Invalid SQLTYPE feature type (i.e. Rect, Arc, Circle) !");break; 
		case -307 :strcpy(strInfo,"\n DBMS Objects (Spatial, ADT's not supported !");break; 
		case -308 :strcpy(strInfo,"\n No columns found for binary conversion (LOB/LONGRAW) !");break;
		case -309 :strcpy(strInfo,"\n The raster band has no colormap. !");break;
		case -310 :strcpy(strInfo,"\n Invalid raster band bin  function. !");break;
		case -311 :strcpy(strInfo,"\n Invalid raster band statistics. !");break;
		case -312 :strcpy(strInfo,"\n Invalid raster band  colormap !");break;
		case -313 :strcpy(strInfo,"\n Invalid raster layer configuration keyword !");break;
		case -314 :strcpy(strInfo,"\n This sort of iomgr can't run on this sort of instance. !");break;
		case -315 :strcpy(strInfo,"\n Export file's volume info is invalid !");break;
		case -316 :strcpy(strInfo,"\n Invalid compression type !");break;
		case -317 :strcpy(strInfo,"\n Invalid index parameter !");break;
		case -318 :strcpy(strInfo,"\n Invalid index type !");break;
		case -319 :strcpy(strInfo,"\n Try to set conflicting value  in object !");break;
		case -320 :strcpy(strInfo,"\n Abstract Data types not supported !");break; 
		case -321 :strcpy(strInfo,"\n No spatial index !");break;
		case -322 :strcpy(strInfo,"\n Name not valid for DBMS !");break;
		case -323 :strcpy(strInfo,"\n ROWID for Oracle Spatial table already exists. !");break;
		case -324 :strcpy(strInfo,"\n gsrvr dll for direct could not be loaded. !");break;
		case -325 :strcpy(strInfo,"\n The table can not be registered. !");break;
		case -326 :strcpy(strInfo,"\n Operation not supported on multiversion table. !");break;
		case -327 :strcpy(strInfo,"\n No ArcSDE server license found !");break;
		case -328 :strcpy(strInfo,"\n Exportfile is not supported !");break;
		case -329 :strcpy(strInfo,"\n Specified table is in use  !");break;
		case -330 :strcpy(strInfo,"\n SE_XMLINDEXINFO object not  initialized !");break;
		case -331 :strcpy(strInfo,"\n Specified XML column exists !");break;
		case -332 :strcpy(strInfo,"\n Tag's data type or alias  doesn't match.!");break;
		case -333 :strcpy(strInfo,"\n XML index already exists. !");break;
		case -334 :strcpy(strInfo,"\n Specified XML index  does not exist. !");break;
		case -335 :strcpy(strInfo,"\n Tried to alter non-modifiable field. !");break;
		case -336 :strcpy(strInfo,"\n Cannot alter/delete template that's referenced by column. !");break;
		case -337 :strcpy(strInfo,"\n Must be owner to del/alter. !");break;
		case -338 :strcpy(strInfo,"\n SE_XMLDOC object not initialized !");break;
		case -339 :strcpy(strInfo,"\n Named tag not found. !");break; 
		case -340 :strcpy(strInfo,"\n Illegal XML document syntax !");break;
		case -341 :strcpy(strInfo,"\n the ZM column does not exists in the layers table !");break;
		case -342 :strcpy(strInfo,"\n Invalid date value !");break;
		case -343 :strcpy(strInfo,"\n Given XML column not found !");break;
		case -344 :strcpy(strInfo,"\n Both XML columns must be the  same for this operation. !");break;
		case -345 :strcpy(strInfo,"\n Incorrect xpath syntax. !");break;
		case -346 :strcpy(strInfo,"\n Input xpath tag not supported  in current release. !");break;
		case -347 :strcpy(strInfo,"\n SE_COLUMNINFO object not initialized !");break;
		case -348 :strcpy(strInfo,"\n SE_XMLTAGINFO object not initialized !");break;
		case -349 :strcpy(strInfo,"\n Wrong type of XML index !");break;
		case -350 :strcpy(strInfo,"\n The requested operation is unsupported by instance db.!");break;
		case -351 :strcpy(strInfo,"\n Incorrect xpath location  alias syntax. !");break;
		case -352 :strcpy(strInfo,"\n Incorrect xpath predicate  token.!");break;
		case -353 :strcpy(strInfo,"\n Incorrect xpath  predicate operator. !");break;
		case -354 :strcpy(strInfo,"\n Incorrect xpath predicate contain function syntax.!");break;
		case -355 :strcpy(strInfo,"\n Empty xpath element is not allowed.!");break;
		case -356 :strcpy(strInfo,"\n Incorrect xpath token. !");break;
		case -357 :strcpy(strInfo,"\n Environment var SDETMP not set to a value   !");break;
		case -358 :strcpy(strInfo,"\n SERVERINFO object not initialized. !");break;
		case -359 :strcpy(strInfo,"\n Ambiguity between a user name and group name when working with permissions !");break;
		case -360 :strcpy(strInfo,"\n Geocoding rule file already exists and force_overwrite is not specified !");break;
		case -361 :strcpy(strInfo,"\n Geocoding rule file is not found !");break;
		case -362 :strcpy(strInfo,"\n Geocoding rule is not found in geocoding rule table !");break;
		case -363 :strcpy(strInfo,"\n Geocoding rule already exist in the geocoding rule table and force_overwrite is not specified !");break;
		case -364 :strcpy(strInfo,"\n SE_XMLCOLUMNINFO object not initialized !");break;
		case -365 :strcpy(strInfo,"\n Invalid system group name !");break;
		case -366 :strcpy(strInfo,"\n Column contains NOT NULL values during SE_xmlcolumn_create() !");break;
		case -367 :strcpy(strInfo,"\n Locator property is too long or invalid !");break;
		case -368 :strcpy(strInfo,"\n Locator property has multiple values !");break;
		case -369 :strcpy(strInfo,"\n Search window completely outside  oracle spatial  geodetic extent !");break;
		case -370 :strcpy(strInfo,"\n Cannot use xpath searches on  non-indexed columns. !");break;
		case -371 :strcpy(strInfo,"\n The current table doesn't haveSDE-maintained UUID column. !");break;
		case -372 :strcpy(strInfo,"\n Invalid UUID column. !");break;
		case -373 :strcpy(strInfo,"\n Underlying RDBMS doesn't  support 64-bit layer. !");break;
		case -374 :strcpy(strInfo,"\n Invalid (rebuild) index mask!");break;
		case -375 :strcpy(strInfo,"\n Coordref/Layer precision not OK for requested operation. !");break;
		case -376 :strcpy(strInfo,"\n The SE_TRANSFORMINFO object  is not initialized !");break;      
		case -377 :strcpy(strInfo,"\n Invalid raster band  transform list !");break;
		case -378 :strcpy(strInfo,"\n The raster band has no transform list !");break;
		case -379 :strcpy(strInfo,"\n SE_QUERYCOLUMN object not initialized !");break;
		case -380 :strcpy(strInfo,"\n Multiple shapes set to SE_SHAPESOURCE shape !");break;
		case -381 :strcpy(strInfo,"\n SE_QUERYSELECT object not initialized !");break;
		case -382 :strcpy(strInfo,"\n SE_SHAPESOURCE object not initialized !");break;
		case -383 :strcpy(strInfo,"\n SE_SPATIALCONSTRAINT object not init'ized !");break;
		case -384 :strcpy(strInfo,"\n QUERYFROM join constraint missing !");break;
		case -385 :strcpy(strInfo,"\n QUERYFROM join conflict !");break;
		case -386 :strcpy(strInfo,"\n QUERYFROM join table missing !");break;
		case -387 :strcpy(strInfo,"\n SE_QUERYTABLE object not initialized !");break;
		case -388 :strcpy(strInfo,"\n SE_QUERYFROM object not initialized !");break;
		case -389 :strcpy(strInfo,"\n SE_XMLCONSTRAINT object not initialized !");break;
		case -390 :strcpy(strInfo,"\n SE_ATTRIBUTECONSTRAINT object not init'ized !");break;
		case -391 :strcpy(strInfo,"\n SE_QUERYWHERE object not initialized !");break;
		case -392 :strcpy(strInfo,"\n Error opening codepage converter !");break;
		case -393 :strcpy(strInfo,"\n The destination string buffer is too small !");break;
		case -394 :strcpy(strInfo,"\n Codepage conversion error !");break;
		case -395 :strcpy(strInfo,"\n Operation invalid on a closed state. !");break;
		case -396 :strcpy(strInfo,"\n The requested operation is invalid on the base state. !");break;
		case -397 :strcpy(strInfo,"\n The direct connect is not registered with an ArcSde server for license. !");break;
		case -398 :strcpy(strInfo,"\n The given range of ids is invalid !");break;
		case -399 :strcpy(strInfo,"\n This state object doesn't contain lineage info. !");break;
		case -400 :strcpy(strInfo,"\n OGCWKB type does not support ANNO, CAD, 3D, Measures or Curves. !");break;
		case -401 :strcpy(strInfo,"\n Not allowed to set image origin. !");break;
		case -402 :strcpy(strInfo,"\n Not a valid image origin. !");break;
		case -403 :strcpy(strInfo,"\n The raster band has no  GDB metadata. !");break;
		case -404 :strcpy(strInfo,"\n Invalid raster band GDB  metadata. !");break;
		case -405 :strcpy(strInfo,"\n Not allowed to create index on complex column. !");break;
		case -406 :strcpy(strInfo,"\n Can't start compress if compress is already running !");break;
		case -407 :strcpy(strInfo,"\n Schema is out of date. !");break;

		case -1000:strcpy(strInfo,"\n BASE NUMBER FOR WARNING CODES!");break;
		case -1001:strcpy(strInfo,"\n FUNCTION CHANGED ENTITY TYPE OF FEAT!");break;
		case -1002:strcpy(strInfo,"\n No rows were deleted!");break;
		case -1003:strcpy(strInfo,"\n Too many distinct values in stats!");break;
		case -1004:strcpy(strInfo,"\n Request column value is NULL!");break;
		case -1005:strcpy(strInfo,"\n No rows were updated !");break;
		case -1006:strcpy(strInfo,"\n No codepage conversion  !");break;
		case -1007:strcpy(strInfo,"\n Cannot find codepage directory!");break;
		case -1008:strcpy(strInfo,"\n DBMS does NOT support this function !");break;
		case -1009:strcpy(strInfo,"\n Invalid DBMS function id!");break;
		case -1010:strcpy(strInfo,"\n Update layer extent failed!");break;
		case -1011:strcpy(strInfo,"\n No localized error messages!");break;
		case -1012:strcpy(strInfo,"\n Spatial index not created,server inability to support SPIDX_PARAM specified!");break;
		case -1013:strcpy(strInfo,"\n SDE does NOT support this function  on this DBMS! ");break;
		case -1014:strcpy(strInfo,"\n SQL statement that is being re-executed was not found in the cache!");break;

		default: strcpy(strInfo,"\n Unkown!");break;
	}

	return strInfo;
}

//==============================class CDistConnection===================================//
CDistConnection::CDistConnection(void)
{
}

CDistConnection::~CDistConnection(void)
{
}

long CDistConnection::CheckConnected()
{
	return 0;
}

// 功能：创建连接
// 返回：成功返回 1，失败返回 0
long CDistConnection::Connect()
{
	return 0;
}

// 功能：断开连接
// 返回：成功返回 1，失败返回 0
long CDistConnection::Disconnect()
{
	return 0;
}

//==============================class CDistConnectSDE===================================//
CDistConnectSDE::CDistConnectSDE(void)
{
	m_enType = C_SDE;
	m_pConnectObj = NULL;
	memset(m_strUserName,0,sizeof(char)*SE_MAX_OWNER_LEN);
	memset(m_strDBName,0,sizeof(char)*SE_MAX_DATABASE_LEN);
}

CDistConnectSDE::~CDistConnectSDE(void)
{
	Disconnect();
}

// 功能：根据返回代码显示相应信息
// 参数： rc          操作返回值
//        szComment   调用函数名称
// 返回：
void CDistConnectSDE::check_error (long rc, char *strComment)
{
	if(rc == SE_SUCCESS || rc == 1) return;
	char strInfo[SE_MAX_MESSAGE_LENGTH] = {0};
	sprintf(strInfo,"\n function:%s, error code:%d message:%s\n",strComment,rc,ReturnErrorCodeInfo(rc));
	OutputDebugString(strInfo);
}

// 功能：获取数据库连接对象指针
// 参数：无
// 返回：返回对象指针
SE_CONNECTION CDistConnectSDE::GetConnectObj(void)
{
	return m_pConnectObj;
}

// 功能：创建连接
// 参数：strServer    服务器名称
//       strInstance  服务实体对象名称
//       strDatabase  数据库名称
//       strUserName  用户名称
//       strPassword  用户密码
// 返回：成功返回 1，失败返回 0，其他参考定义中的DIST_ENUM_CONNECTRESULT
long CDistConnectSDE::Connect(const char* strServer,
							 const char* strInstance,
							 const char* strDatabase,
		                     const char* strUserName,
						     const char* strPassword)
{
	SE_ERROR sdeError;

	//考虑到用C-API获取，存在远程不能读取参数的原因，所以在此保存以备后用
	strcpy(m_strDBName,strDatabase);   
	strcpy(m_strUserName,strUserName);

	long rc = -1;
	try{
		rc = SE_connection_create(strServer,strInstance,strDatabase,strUserName,strPassword, &sdeError, &m_pConnectObj);
		check_error(rc,"Connect->SE_connection_create");
	}
	catch (...) {
		check_error(-1,"系统异常，连接数据库失败!");
		return -1;
	}

	if(rc == SE_SUCCESS)
		return 1;
	else
		return rc;	
}   


// 功能：断开连接
// 参数：无
// 返回：无
long CDistConnectSDE::Disconnect(void)
{
	if(NULL != m_pConnectObj)
	{
		SE_connection_free(m_pConnectObj);
		m_pConnectObj = NULL;
	}
	
	return 1;
}


// 功能：检查连接
// 参数：IntervalTime检测超时
// 返回：TRUE 连接成功，FALSE 连接失败
long CDistConnectSDE::CheckConnected(int IntervalTime)
{
	long rc = SE_connection_test_server(m_pConnectObj, IntervalTime);
	
	if(rc == SE_SUCCESS)
		return 1;
	else
	{
		check_error(rc,"CheckConnect->SE_connection_test_server");
	    return rc;
	}
}

//==============================class CDistConnectMIF===================================//
CDistConnectMIF::CDistConnectMIF(void)
{
}

CDistConnectMIF::~CDistConnectMIF(void)
{
}

//==============================class CDistConnectSHAPE===================================//
CDistConnectSHAPE::CDistConnectSHAPE(void)
{
}

CDistConnectSHAPE::~CDistConnectSHAPE(void)
{
}
