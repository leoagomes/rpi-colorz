require 'pstore'

require_relative '../helpers/mode_router'

module Colors
  module Services
    class ModeService
      def initialize(store)
        @store = store
        @current = nil

        @router = ModeRouter.new ENV['MODE_PUB_HOST']
      end

      def all
        modes = []
        @store.transaction(true) do
          modes = @store.roots.map { |mode_id| @store[mode_id] }
        end
        modes
      end

      def save(mode)
        @store.transaction do
          @store[mode['id']] = mode
        end
      end

      def find(id)
        mode = nil
        @store.transaction do
          mode = @store[id]
        end
        mode
      end

      def exists?(id)
        exists = false
        @store.transaction do
          if @store.roots.key id
            exists = true
          end
        end
        exists
      end

      def update(id, data)
        mode = nil
        if exists? id
          mode = find id

          data.keys.each do |key|
            mode[key] = data[key]
          end

          save mode
        end
        mode
      end

      def delete(id)
        if @store.fetch id, false
          @store.delete id
          true
        else
          false
        end
      end

      def current
        @current
      end

      def current=(arg)
        @current = arg
        @router.send_mode(arg)
        @current
      end

      def update_current_parameters(data)
        if current == nil
          raise 'Current not found'
        end

        data.each do |key, value|
          current['params'][key]['value'] = value
        end

        @router.send_params(data)

        current
      end
    end

  end
end