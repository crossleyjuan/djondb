#include "mysqlresultset.h"
#include <sstream>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string.h>

MySQLResultSet::MySQLResultSet(MYSQL_RES* res, const char* query) {
    m_res = res;
    m_query = query;

    m_res_numfields = mysql_num_fields(res);
    m_res_fields = (MYSQL_FIELD*) malloc(sizeof (MYSQL_FIELD) * m_res_numfields);
    m_res_fieldsByName = new map<string, int>();
    for (int i = 0; i < m_res_numfields; i++) {
        MYSQL_FIELD* field = mysql_fetch_field(res);
        m_res_fields[i] = *field;
        string name(field->name);
        m_res_fieldsByName->insert(pair<string, int>(name, i));
    }
};

MySQLResultSet::~MySQLResultSet() {
}

bool MySQLResultSet::next() throw (DBException) {
    m_currentRow = mysql_fetch_row(m_res);
    if (m_currentRow) {
        return true;
    } else {
        return false;
    }
};

bool MySQLResultSet::previous() throw (DBException) {
    return true;
};

void MySQLResultSet::close() throw (DBException) {
    while ((m_currentRow = mysql_fetch_row(m_res)));

    m_res_fieldsByName->clear();
    delete m_res_fieldsByName;

    mysql_free_result(m_res);
    free(m_res_fields);
}

void* MySQLResultSet::get(int col) throw (DBException) {
    char* value = (char*) m_currentRow[col];
    MYSQL_FIELD field = m_res_fields[col];
    void* res = NULL;
    switch (field.type) {
        case MYSQL_TYPE_BIT:
        {
            int ivalue = atoi(value);
            bool* bres;
            *bres = (ivalue == 1) ? true : false;
            res = bres;
            break;
        }
        case MYSQL_TYPE_DOUBLE:
        {
            double* dvalue = (double*) malloc(sizeof (double));
            std::stringstream ss(value);
            ss >> *dvalue;
            res = dvalue;
            break;
        }
        case MYSQL_TYPE_FLOAT:
        {
            float* fvalue = (float*) malloc(sizeof (double));
            std::stringstream ss(value);
            ss >> *fvalue;
            res = fvalue;
            break;
        }
        case MYSQL_TYPE_INT24:
        case MYSQL_TYPE_TINY:
        {
            int *ivalue = (int*) malloc(sizeof (int));
            *ivalue = atoi(value);
            res = ivalue;
            break;
        }
        case MYSQL_TYPE_LONG:
        {
            long *lvalue = (long*) malloc(sizeof (int));
            *lvalue = atol(value);
            res = lvalue;
            break;
        }
        case MYSQL_TYPE_SHORT:
        {
            short int* shValue = (short int*) malloc(sizeof (short int));
            *shValue = (short int) atoi(value);
            res = shValue;
            break;
        }
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_VAR_STRING:
        {
            const char* resString = value;
            return new string(resString);
        }
        case MYSQL_TYPE_VARCHAR:
            res = value;
            break;

        case MYSQL_TYPE_BLOB:
        case MYSQL_TYPE_DATE:
        case MYSQL_TYPE_DATETIME:
        case MYSQL_TYPE_DECIMAL:
        case MYSQL_TYPE_ENUM:
        case MYSQL_TYPE_GEOMETRY:
        case MYSQL_TYPE_LONGLONG:
        case MYSQL_TYPE_LONG_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_NEWDATE:
        case MYSQL_TYPE_NEWDECIMAL:
        case MYSQL_TYPE_NULL:
        case MYSQL_TYPE_SET:
        case MYSQL_TYPE_TIME:
        case MYSQL_TYPE_TIMESTAMP:
        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_YEAR:
            throw DBException(new string("Not supported"));
    }
    return res;
}

void* MySQLResultSet::get(const char* colname) throw (DBException) {
    map<string, int>::iterator it = m_res_fieldsByName->find(string(colname));
    if (it == m_res_fieldsByName->end()) {
        return NULL;
    } else {
        return get(it->second);
    }
}

