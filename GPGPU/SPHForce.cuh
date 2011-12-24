

struct DeviceForceDataSet
{
    sphForcePointGravity *sgravity;
};


struct ForceDataSet
{
    thrust::device_vector<sphForcePointGravity> sgravities;

    ForceDataSet()
    {
        sgravities.reserve(SPH_MAX_SPHERICAL_GRAVITY_NUM);
        sgravities.resize(1);
    }

    DeviceForceDataSet getDeviceData()
    {
        DeviceForceDataSet ddata;
        ddata.sgravity  = sgravities.data().get();
        return ddata;
    }
};
