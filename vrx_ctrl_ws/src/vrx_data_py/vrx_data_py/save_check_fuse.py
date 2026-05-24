import rclpy
from rclpy.node import Node
import numpy as np

from vrx_other_interface.msg import State
from sensor_msgs.msg import Imu, NavSatFix


class Check_Fuse_Saver(Node):

    def __init__(self):
        super().__init__('Check_Fuse_Saver')
        self.subscription_fuse = self.create_subscription(
            State, "/state", self.fuse_callback, 10)
        self.subscription_imu = self.create_subscription(
            Imu, "/wamv/sensors/imu/imu/data", self.imu_callback, 10)
        self.subscription_gps = self.create_subscription(
            NavSatFix, "/wamv/sensors/gps/gps/fix", self.gps_callback, 10)

        self.subscription_fuse  # prevent unused variable warning
        self.subscription_imu
        self.subscription_gps
        
        self.lat0 = -33.7227687
        self.lon0 = 150.67399115
        self.RE = 6371393
        
        self.declare_parameter('Length_of_Data', 1000) 
        self.len_data = self.get_parameter('Length_of_Data').value
        
        self.count = 0
        self.fuse_data = np.zeros((self.len_data, 10))
        self.imu_data = np.array([[0] * 10]).astype(np.float)
        self.imu_data[0, 3] = 0.91424		# psi0
        self.gps_data = [[0, 0, 0]]
        self.aub = -1.554e-1
        self.avb = -5.342e-2
        self.wzb = -2.974e-4
        
        self.saved_imu = False
        self.saved_gps = False

    def fuse_callback(self, msg):

    	if self.count < len(self.fuse_data):
            self.fuse_data[self.count, 0] = msg.time
            self.fuse_data[self.count, 1] = msg.pos.x
            self.fuse_data[self.count, 2] = msg.pos.y
            self.fuse_data[self.count, 3] = msg.pos.psi
            self.fuse_data[self.count, 4] = msg.vel.u
            self.fuse_data[self.count, 5] = msg.vel.v
            self.fuse_data[self.count, 6] = msg.vel.psi
            self.fuse_data[self.count, 7] = msg.acc.u
            self.fuse_data[self.count, 8] = msg.acc.v
            self.fuse_data[self.count, 9] = msg.acc.psi

            self.get_logger().info('Count "%d"' % self.count)

    	if self.count == len(self.fuse_data):
            np.savetxt('/home/cizar/Desktop/NeedRename_Fuse.txt', self.fuse_data)
            self.get_logger().info('Save FUSE TXT')

    	self.count += 1

    def imu_callback(self, msg):

    	if self.count < len(self.fuse_data):
    	    sec = msg.header.stamp.sec
    	    nanosec = msg.header.stamp.nanosec
    	    time = sec + nanosec/1000000000.0

    	    au = msg.linear_acceleration.x # - self.aub
    	    av = msg.linear_acceleration.y # - self.avb
    	    wz = msg.angular_velocity.z # - self.wzb

    	    last_data = self.imu_data[-1]
    	    dt = time - last_data[0]
    	    
    	    acc_wz = (wz - last_data[6]) / dt
    	    psi = last_data[3] + last_data[6]*dt
    	    
    	    u = last_data[4] + au * dt
    	    v = last_data[5] + av * dt
    	    x = last_data[1] + (np.cos(psi)*u - np.sin(psi)*v) * dt
    	    y = last_data[2] + (np.sin(psi)*u + np.cos(psi)*v) * dt
    	    
    	    now_data = np.array([[time, x, y, psi, u, v, wz, au, av, acc_wz]])

    	    self.imu_data = np.vstack([self.imu_data, now_data])

    	if self.count >= len(self.fuse_data) and not self.saved_imu:
            np.savetxt('/home/cizar/Desktop/NeedRename_Imu.txt', self.imu_data)
            self.get_logger().info('Save IMU TXT')
            self.saved_imu = True

    def gps_callback(self, msg):
    	sec = msg.header.stamp.sec
    	nanosec = msg.header.stamp.nanosec
    	time = sec + nanosec/1000000000.0
    	
    	latitude = msg.latitude
    	longitude = msg.longitude
    	
    	x = (longitude - self.lon0) / 180 * np.pi * self.RE
    	y = (latitude - self.lat0)  / 180 * np.pi * self.RE

    	if self.count < len(self.fuse_data):
            self.gps_data += [[time, x, y]]

    	if self.count >= len(self.fuse_data) and not self.saved_gps:
    	    save_data = np.array(self.gps_data)
    	    np.savetxt('/home/cizar/Desktop/NeedRename_Gps.txt', save_data)
    	    self.get_logger().info('Save GPS TXT')
    	    self.saved_gps = True


def main(args=None):
    rclpy.init(args=args)

    minimal_subscriber = Check_Fuse_Saver()

    rclpy.spin(minimal_subscriber)

    minimal_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
