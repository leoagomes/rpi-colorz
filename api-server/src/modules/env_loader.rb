def env_defaults(ht)
  ht.each_pair do |key, value|
    ENV[key] = value unless ENV.has_key? key
  end
end