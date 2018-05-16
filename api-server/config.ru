require 'rubygems'
require 'bundler'
require 'rack/protection'

require './app'

Bundler.require

use Rack::Protection::PathTraversal

# TODO: use David

run Colors::App
