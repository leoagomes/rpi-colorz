require 'socket'

module DeviceProtocol
  OP_BUFFER_START_SET = 0x00
  OP_BUFFER_REPLACE = 0x01
  OP_BUFFER_INSERT = 0x02
  OP_BUFFER_ROTATE = 0x03
  OP_BUFFER_SHIFT = 0x04
  OP_BUFFER_FILL = 0x05
  OP_BUFFER_SET_INDEX = 0x06

  OP_BUFFER_READ = 0x10

  OP_STRIP_GET_COUNT = 0x20
  OP_STRIP_SET_COUNT = 0x21
  OP_STRIP_GET_STATE = 0x22
  OP_STRIP_SET_STATE = 0x23

  OP_DEVICE_GET_STRIP_COUNT = 0xFE
  OP_VENDOR_SPECIFIC = 0xFF
end

def color_i(w, r, g, b)
  w << 24 | r << 16 | g << 8 | b
end

class Integer
  def unpack_color
    val = to_int
    [val >> 24 & 0xFF, val >> 16 & 0xFF, val >> 8 & 0xFF, val & 0xFF]
  end
end

class Array
  def pack_color
    color_i self[0], self[1], self[2], self[3]
  end
end

class DeviceClient
  attr_accessor :address
  attr_accessor :port
  attr_accessor :socket

  def initialize(address, port)
    @address = address
    @port = port
  end

  def connect
    @socket = TCPSocket.new address, port
  end

  def send(data)
    begin
      @socket.send data, 0
    rescue
      connect
      @socket.send data, 0
    end
  end

  def recv(amount)
    @socket.recv amount
  end

  def get_length(strip)
    data = [DeviceProtocol::OP_STRIP_GET_COUNT, strip.to_int].pack 'CC'
    send data
    recv(2).unpack('n').first
  end

  def set_length(strip, length)
    data = [DeviceProtocol::OP_STRIP_SET_COUNT, strip.to_int, length].pack 'CCn'
    send data
  end

  def set_state(strip, state)
    state_value = if state == 'on' then 1 else 0 end
    data = [DeviceProtocol::OP_STRIP_SET_STATE, strip.to_int, state_value].pack 'CCC'
    send data
  end

  def get_state(strip)
    data = [DeviceProtocol::OP_STRIP_GET_STATE, strip.to_int].pack 'CC'
    send data
    response = recv(1).unpack('C').first
    if response == 0
      'off'
    else
      'on'
    end
  end

  def get_strip_count
    data = [DeviceProtocol::OP_DEVICE_GET_STRIP_COUNT, 0].pack 'CC'
    send data
    recv(1).unpack('C').first
  end

  def start_set(strip, buffer)
    data_array = [DeviceProtocol::OP_BUFFER_START_SET, strip.to_int]
    data_array << buffer.length << buffer
    data = data_array.flatten.pack 'CCnN*'
    send data
  end

  def replace(strip, buffer, start)
    data_array = [DeviceProtocol::OP_BUFFER_REPLACE, strip.to_int]
    data_array << start.to_int << buffer.length << buffer
    data = data_array.flatten.pack 'CCnnN*'
    send data
  end

  def insert(strip, buffer, start)
    data_array = [DeviceProtocol::OP_BUFFER_INSERT, strip.to_int]
    data_array << start.to_int << buffer.length << buffer
    data = data_array.flatten.pack 'CCnnN*'
    send data
  end

  def rotate(strip, amount)
    data = [DeviceProtocol::OP_BUFFER_ROTATE, strip.to_int, amount].pack 'CCs>'
    send data
  end

  def shift(strip, amount, color)
    data = [DeviceProtocol::OP_BUFFER_SHIFT, strip.to_int, amount, color].pack 'CCs>N'
    send data
  end

  def fill(strip, color)
    data = [DeviceProtocol::OP_BUFFER_FILL, strip.to_int, color].pack 'CCN'
    send data
  end

  def set_index(strip, index, color)
    data = [DeviceProtocol::OP_BUFFER_SET_INDEX, strip.to_int, index, color].pack 'CCnN'
    send data
  end

  def read(strip)
    length = get_length(strip)

    if length <= 0 then return [] end

    data = [DeviceProtocol::OP_BUFFER_READ, strip.to_int].pack 'CC'
    send data
    recv(length * 4).unpack('N*')
  end
end