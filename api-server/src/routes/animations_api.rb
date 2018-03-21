require 'sinatra'
require_relative '../modules/animation_router'

class AnimationsApi < Sinatra::Base
  def initialize
    @router = AnimationRouter.new
    @router.start ENV['ANIMATION_ROUTER_HOST']
  end

  get '/animations' do
  end

  get '/animations/:id' do |id|
  end

end