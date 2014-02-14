# hdfinfo

A very basic command line utility for extracting information about a HDF file. This is pretty rough and was written so I could quickly find out about variable names, dimensions etc in some HDF files I was working with.

## Usage

```
hdfinfo <hdf file name> [<output file name>]
```

If the second argument is ommitted output is written to the console.

## Dependencies

This Depends on the [NetCDF](http://www.unidata.ucar.edu/software/netcdf/) library (downloads [here](http://www.unidata.ucar.edu/downloads/netcdf/index.jsp)).

## Notes

- This is written using the [NetCDF C API](https://www.unidata.ucar.edu/software/netcdf/docs/netcdf-c/).
- I'm not sure if I'm doing things right when extracting attribute strings. In the files I was interrogating when I wrote this, the attribute strings had `nc_type` of `NC_CHAR`, but it seems strange that one can't distinguish between a 'string' and a 'char'. I use [`nc_get_att_text`](http://www.unidata.ucar.edu/software/netcdf/docs/netcdf-c/nc_005fget_005fatt_005f-type.html) for reading attributes of type `NC_CHAR`, and [`nc_get_vara_uchar`](http://www.unidata.ucar.edu/software/netcdf/docs/netcdf-c/nc_005fget_005fvara_005f-type.html) when reading values from (dimension) variables... This probably needs some work.
- This only deals with some of the `nc_types`; specifically `NC_BYTE`, `NC_CHAR`, `NC_SHORT`, `NC_INT`, `INC_FLOAT` and `NC_DOUBLE` and as noted in the point above, `NC_CHAR` is treated as a string (for attributes at least).
