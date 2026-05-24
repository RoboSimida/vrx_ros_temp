from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.launch_context import LaunchContext
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution, LaunchConfiguration

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    # context = LaunchContext()
    Pi = 3.1415926
    
    Test_Fuse_ns_launch_arg = DeclareLaunchArgument(
        'test_fuse_ns', 
        default_value='test_fuse')
    test_fuse_ns = LaunchConfiguration('test_fuse_ns')
        
    Go_Node1 = Node(
        package = 'vrx_experiment',
        namespace = test_fuse_ns,
        executable = 'Fang',
        name = 'Go_Ctrl',
        parameters=[
            {'Ang': Pi/72},
        ]
    )
    
    Go_Node2 = Node(
        package = 'vrx_controller',
        namespace = test_fuse_ns,
        executable = 'Go_Ctrl', 
        name = 'Input'
    )
        
    Check_Fuse_Node = Node(
        package = 'vrx_data_py',
        namespace = test_fuse_ns,
        executable = 'Check_Fuse_Saver',
        name = 'Check_Fuse_Saver',
        remappings = [
            ('/state', 
            '/test_fuse/state'),
        ],
        parameters=[
            {'Length_of_Data': 500},
        ]
    )
    
    # print(test_fuse_ns)
    # print(test_fuse_ns.perform(context))
      
    Fuse_Node = Node(
        package = 'vrx_data_cpp',
        namespace = test_fuse_ns,
        executable = 'gps_imu_fuse',
        name = 'Gps_Imu_Fuse'
    )
        
    Vrx_Launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare('vrx_gz'),
                'launch', 'competition.launch.py'])
        ]),
    )
    
    return LaunchDescription([
    	Test_Fuse_ns_launch_arg,
    	Go_Node1,
    	Check_Fuse_Node,
    	Fuse_Node,
    	Vrx_Launch,  	        
    ])
