from setuptools import find_packages, setup

package_name = 'vrx_data_py'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='cizar',
    maintainer_email='cizar@todo.todo',
    description='TODO: Package description',
    license='Apache-2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'Gps_Saver = vrx_data_py.save_gps:main',
            'Imu_Saver = vrx_data_py.save_imu:main',
            'Check_Fuse_Saver = vrx_data_py.save_check_fuse:main',
            'JointState_Saver = vrx_data_py.save_joint:main',
            'IRLS1_Saver = vrx_data_py.save_IRLS_straight:main',
            'Input_Saver = vrx_data_py.save_input:main',
            'IRLS2_Saver = vrx_data_py.save_IRLS_other:main',
            'Data_Saver = vrx_data_py.save_all:main',
            'Data2_Saver = vrx_data_py.save_no_error:main',
        ],
    },
)
