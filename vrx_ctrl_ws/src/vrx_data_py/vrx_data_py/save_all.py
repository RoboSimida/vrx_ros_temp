import rclpy
from rclpy.node import Node
import numpy as np

from sensor_msgs.msg import JointState
from vrx_other_interface.msg import State, Losmsg
from std_msgs.msg import Float64


class All_Saver(Node):

	def __init__(self):
		super().__init__('All_Saver')
		self.joint_sub = self.create_subscription(
			JointState, '/wamv/joint_states', self.joint_callback, 10)
		self.state_sub = self.create_subscription(
			State, '/state', self.state_callback, 10)
		self.error_sub = self.create_subscription(
			Losmsg, '/error', self.error_callback, 10)
			
		self.joint_sub   # prevent unused variable warning
		self.state_sub
		self.error_sub
		
		self.count = 0
		
		self.declare_parameter('Length_of_Data', 10000)
		self.declare_parameter('End_LOS', True)
		
		self.len_data = self.get_parameter('Length_of_Data').value
		self.End_LOS = self.get_parameter('End_LOS').value
		self.saved = False
		
		self.data_state = []
		self.data_joint = []
		self.data_error = []
		
		
	def state_callback(self, msg):
		self.count += 1
		if self.count < self.len_data and not self.saved:
			self.data_state.append([msg.time, 
						msg.pos.x, msg.pos.y, msg.pos.psi,
						msg.vel.u, msg.vel.v, msg.vel.psi,
						msg.acc.u, msg.acc.v, msg.acc.psi])
			self.get_logger().info('Count "%d x%f y%f"' % (self.count, msg.pos.x, msg.pos.y))
			
				
	def joint_callback(self, msg):
		if self.count < self.len_data:
			self.data_joint.append([msg.position[4], msg.velocity[5]])

		
	def error_callback(self, msg):
		self.data_error.append(msg.error)

		if self.End_LOS:
			if (msg.end or self.count == self.len_data) and not self.saved: 
				np.savetxt('/home/cizar/Desktop/NeedRename_state.txt', np.array(self.data_state))
				np.savetxt('/home/cizar/Desktop/NeedRename_joint.txt', np.array(self.data_joint))
				np.savetxt('/home/cizar/Desktop/NeedRename_error.txt', np.array(self.data_error))
				self.get_logger().info('Save Txt')
				self.saved = True
		else:
			if self.count == self.len_data:
				np.savetxt('/home/cizar/Desktop/NeedRename_state.txt', np.array(self.data_state))
				np.savetxt('/home/cizar/Desktop/NeedRename_joint.txt', np.array(self.data_joint))
				np.savetxt('/home/cizar/Desktop/NeedRename_error.txt', np.array(self.data_error))
				self.get_logger().info('Save Txt')
				self.saved = True
			
def main(args=None):
	rclpy.init(args=args)

	minimal_subscriber = All_Saver()

	rclpy.spin(minimal_subscriber)
	minimal_subscriber.destroy_node()
	rclpy.shutdown()


if __name__ == '__main__':
	main()
