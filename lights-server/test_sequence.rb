require '../api-server/src/modules/device_client'

address = 'raspberrypi.local'
port = 7890

dev = DeviceClient.new address, port
dev.connect

puts "Connected to #{address}:#{port}."

puts "Setting strip on."
dev.set_state 0, 'on'

puts "Filling strip RED"
dev.fill 0, color_i(0, 255, 0, 0)

sleep 2

puts "Filling strip GREEN"
dev.fill 0, color_i(0, 0, 255, 0)

sleep 2

puts "Filling strip BLUE"
dev.fill 0, color_i(0, 0, 0, 255)

sleep 2

puts "Shifting green POSITIVE"
dev.shift 0, +3, color_i(0, 0, 255, 0)

sleep 2

puts "Shifting white NEGATIVE"
dev.shift 0, -3, color_i(0, 0, 255, 0)