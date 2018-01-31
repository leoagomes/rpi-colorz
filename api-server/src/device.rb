#
class Device
  attr_accessor :state

  def initialize
    @state = 'off'
  end

  def turn(state)
    @state = state
    # TODO: send data
  end
end
