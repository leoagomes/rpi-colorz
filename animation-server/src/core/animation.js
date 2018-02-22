animations = {
	running: null,
	loaded: [],
	runners: {
		lua: require('../runners/lua'),
		forth: require('../runners/forth'),
		js: require('../runners/js')
	}
}

animations.load = function (metadata, script) {
	let item = {}
	item.meta = metadata.meta;
	item.script = script;
	item.paramters = metadata.parameters || metadata.meta.parameters.default
		|| {};

	animations.runners.push(item);
}