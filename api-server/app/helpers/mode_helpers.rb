
module Rainbow
  module Helpers
    module Mode
      def valid_mode?(map)
        begin
          if not map["name"].is_a? String or
              map["name"].empty?
            return false
          end

          if not map["id"].is_a? String or
              map["id"].empty? or
              map["id"] == "current"
            return false
          end

          unless map.key? "engine"
            return false
          end

          unless map.key? "data"
            return false
          end

          unless map["params"].is_a? Hash
            return false
          end

          return true
        rescue
          return false
        end
      end
    end
  end
end
