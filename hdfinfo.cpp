//             Copyright Simon Knapp 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <netcdf.h>

using namespace std;



static int doGetAttrValue(int ncid, int varid, char const* name, double* result) {
    return nc_get_att_double(ncid, varid, name, result);
}

static int doGetAttrValue(int ncid, int varid, char const* name, float* result) {
    return nc_get_att_float(ncid, varid, name, result);
}

static int doGetAttrValue(int ncid, int varid, char const* name, unsigned char* result) {
    return nc_get_att_uchar(ncid, varid, name, result);
}

static int doGetAttrValue(int ncid, int varid, char const* name, signed char* result) {
    return nc_get_att_schar(ncid, varid, name, result);
}

static int doGetAttrValue(int ncid, int varid, char const* name, short* result) {
    return nc_get_att_short(ncid, varid, name, result);
}

static int doGetAttrValue(int ncid, int varid, char const* name, int* result) {
    return nc_get_att_int(ncid, varid, name, result);
}

template<typename T>
static void attrValue(
    ostream& out,
    int ncid,
    int varid,
    char const* name,
    size_t len
) {
    vector<T> result(len+1);
    int retval;
    if(retval = doGetAttrValue(ncid, varid, name, &result[0])) {
    out << "error getting value for attribute: '" << name << "': " << nc_strerror(retval);
    } else {
        for(size_t i(0); i<len; ++i) {
            out << result[i];
            if(i!=len-1) out << ", ";
        }
    }
    out << endl;
}

template<>
static void attrValue<char>(
    ostream& out,
    int ncid,
    int varid,
    char const* name,
    size_t len
) {
    vector<char> result(len+1);
    int retval;
    if(retval = nc_get_att_text(ncid, varid, name, &result[0])) {
        out << "error getting value for attribute '" << name << "': " << nc_strerror(retval);
    } else {
        result[len] = '\0';
        out << &result[0];
    }
    out << endl;
}

static void dumpAttrValue(ostream& out, int ncId, int varId, char const* name, size_t len, nc_type ncType) {
    switch(ncType) {
        case NC_BYTE:
            attrValue<signed char>(out, ncId, varId, name, len);
            break;
        case NC_CHAR:
            attrValue<char>(out, ncId, varId, name, len);
            break;
        case NC_SHORT:
            attrValue<short>(out, ncId, varId, name, len);
            break;
        case NC_INT:
            attrValue<int>(out, ncId, varId, name, len);
            break;
        case NC_FLOAT:
            attrValue<float>(out, ncId, varId, name, len);
            break;
        case NC_DOUBLE:
            attrValue<double>(out, ncId, varId, name, len);
            break;
        default:
            out << "data type not supported";
    }
}



static int doReadVar(int ncid, int varid, size_t const starts[], size_t const lengths[], signed char* val) {
    return nc_get_vara_schar(ncid, varid, starts, lengths, val);
}

static int doReadVar(int ncid, int varid, size_t const starts[], size_t const lengths[], unsigned char* val) {
    return nc_get_vara_uchar(ncid, varid, starts, lengths, val);
}

static int doReadVar(int ncid, int varid, size_t const starts[], size_t const lengths[], short* val) {
    return nc_get_vara_short(ncid, varid, starts, lengths, val);
}

static int doReadVar(int ncid, int varid, size_t const starts[], size_t const lengths[], int* val) {
    return nc_get_vara_int(ncid, varid, starts, lengths, val);
}

static int doReadVar(int ncid, int varid, size_t const starts[], size_t const lengths[], long* val) {
    return nc_get_vara_long(ncid, varid, starts, lengths, val);
}

static int doReadVar(int ncid, int varid, size_t const starts[], size_t const lengths[], float* val) {
    return nc_get_vara_float(ncid, varid, starts, lengths, val);
}

static int doReadVar(int ncid, int varid, size_t const starts[], size_t const lengths[], double* val) {
    return nc_get_vara_double(ncid, varid, starts, lengths, val);
}

template<typename T>
static void readDimValue(
    ostream& out,
    int ncid,
    int varid,
    size_t dimStart
) {
    T val;
    int retval;
    size_t starts[] = {dimStart};
    size_t lens[] = {1};
    if(retval = doReadVar(ncid, varid, starts, lens, &val)) {
        out << "errort getting value for dimension variable with id " << varid << ": " << nc_strerror(retval) << endl;
    } else {
        out << val;
    }
}

static void dumpDimValue(ostream& out, int ncId, int varId, size_t element, nc_type ncType) {
    switch(ncType) {
        case NC_BYTE:
            readDimValue<signed char>(out, ncId, varId, element);
            break;
        case NC_CHAR:
            readDimValue<unsigned char>(out, ncId, varId, element);
            break;
        case NC_SHORT:
            readDimValue<short>(out, ncId, varId, element);
            break;
        case NC_INT:
            readDimValue<int>(out, ncId, varId, element);
            break;
        case NC_FLOAT:
            readDimValue<float>(out, ncId, varId, element);
            break;
        case NC_DOUBLE:
            readDimValue<double>(out, ncId, varId, element);
            break;
        default:
            out << "data type not supported";
    }
}

static void dumpDim(ostream& out, int ncId, int dimId) {
    char name[NC_MAX_NAME + 1];
    size_t len;
    int retVal, varId;
    nc_type ncType;

    if(retVal = nc_inq_dim(ncId, dimId, name, &len)) {
        out << "error getting information for dimension with id " << dimId << ": " << nc_strerror(retVal) << endl;
        return;
    } else {
        out << "\t" << name << " (length=" << len;
    }

    if(retVal = nc_inq_varid(ncId, name, &varId)) {
        out << ")" << endl << "error getting id of dimension variable with name '" << name << "': " << nc_strerror(retVal) << endl;
    } else if(retVal = nc_inq_vartype(ncId, varId, &ncType)) {
        out << ")" << endl << "error getting data type of dimension variable with name '" << name << "': " << nc_strerror(retVal) << endl;
    } else {
        out << ", range=(";
        dumpDimValue(out, ncId, varId, 0, ncType);
        out << ", ";
        dumpDimValue(out, ncId, varId, len-1, ncType);
        out << "))";
    }
}



static void dumpDataType(ostream& out, nc_type ncType) {
    switch(ncType) {
        case NC_BYTE:
            out << "NC_BYTE";
            break;
        case NC_CHAR:
            out << "NC_CHAR";
            break;
        case NC_SHORT:
            out << "NC_SHORT";
            break;
        case NC_INT:
            out << "NC_INT";
            break;
        case NC_FLOAT:
            out << "NC_FLOAT";
            break;
        case NC_DOUBLE:
            out << "NC_DOUBLE";
            break;
        default:
            out << "UNKNOWN";
    }
}



static void dumpVariable(ostream& out, int ncId, int varId) {
    char name[NC_MAX_NAME + 1];
    nc_type ncType;
    int retVal, nDims, dimIds[NC_MAX_VAR_DIMS], nAttrs;
    if(retVal = nc_inq_var(ncId, varId, name, &ncType, &nDims, dimIds, &nAttrs)) {
        out << "error getting information for variable with id " << varId << ": " << nc_strerror(retVal) << endl;
        throw runtime_error("");
    }

    out <<
        "variable: " << name << " (id " << varId << ")" << endl <<
        "\tdata type: "; dumpDataType(out, ncType); out << endl <<
        "\tdimensions: ";

    // dimensions
    for(int i(0); i<nDims; ++i) {
        if(retVal = nc_inq_dimname(ncId, dimIds[i], name)) {
            out << "error getting name of dimension with id " << dimIds[i] << ": " << nc_strerror(retVal) << endl;
        } else {
            out << name << " ";
        }
    }

    // attributes
    if(nAttrs > 0) {
        out << endl << "\tattributes (" << nAttrs << ")" << endl;
        size_t len;
        for(int i(0); i<nAttrs; ++i) {
            if(retVal = nc_inq_attname(ncId, varId, i, name)) {
                out << "error getting name of attribute number " << i << " of variable with id " << varId << ": " << nc_strerror(retVal) << endl;
            } else if(retVal = nc_inq_att(ncId, varId, name, &ncType, &len)) {
                out << "error getting information about attribute with name '" << name << "' for variable with id " << varId << ": " << nc_strerror(retVal) << endl;
            } else {
                out << "\t\t" << name << ": ";
                //out << "\t\t" << name << " (type=" << static_cast<int>(ncType) << ", length=" << len << ")" << endl;
                dumpAttrValue(out, ncId, varId, name, len, ncType);
            }
        }
    } else {
        out << "no attributes" << endl;
    }
}



int main(int argc, char* argv[]) {
    int
        retVal;

    int
        ncId;

    int
        nDims,
        nVars,
        nAttrs,
        unlimDimId;

    size_t
        len;

    nc_type
        ncType;

    char
        name[NC_MAX_NAME + 1];

    ofstream fout;

    if(argc > 2) fout.open(argv[2]);

    ostream&
        out = (argc > 2 ? fout : cout);

    char const
        *datasetName(argv[1]);

    // open the dataset
    if(retVal = nc_open(datasetName, NC_NOWRITE, &ncId)) {
        out << "error opening '" << argv[1] << "': " << nc_strerror(retVal) << endl;
        return 1;
    }

    // get some information about the dataset
    if(retVal = nc_inq(ncId, &nDims, &nVars, &nAttrs, &unlimDimId)) {
        out << "error getting dataset description: " << nc_strerror(retVal) << endl;
        return 2;
    }

    // dump each dimension
    out << "dimensions" << endl << "----------" << endl;
    for(int i(0); i<nDims; ++i) {
        dumpDim(out, ncId, i);
        out << endl;
    }
    out << endl << endl;

    // dump each variable
    out << "variables" << endl << "---------" << endl;
    for(int i(0); i<nVars; ++i) {
        dumpVariable(out, ncId, i);
        out << endl;
    }
    out << endl << endl;

    // dump global attributes
    if(nAttrs > 0) {
        out << "global attributes (" << nAttrs << ")" << endl << "-----------------" << endl;
        for(int i(0); i<nAttrs; ++i) {
            if(retVal = nc_inq_attname(ncId, NC_GLOBAL, i, name)) {
                out << "error getting name of global attribute number " << i << ": " << nc_strerror(retVal) << endl;
            } else if(retVal = nc_inq_att(ncId, NC_GLOBAL, name, &ncType, &len)) {
                out << "error getting global attribute with name '" << name << "': " << nc_strerror(retVal) << endl;
            } else {
                out << "\t\t" << name << ": ";
                dumpAttrValue(out, ncId, NC_GLOBAL, name, len, ncType);
            }
        }
    }

	return 0;
}
