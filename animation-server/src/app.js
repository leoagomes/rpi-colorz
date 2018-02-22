const express = require('express');
const app = express();
let bodyParser = require('body-parser');

env = require('./env');

animation = require('./core/animation');
strip = require('./core/strip');

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

let animationsRoute = require('./routes/animations');
let environmentRoute = require('./routes/environment');

app.use('/animations', animationsRoute);
app.use('/env', environmentRoute);

app.listen(3000, () => console.log('started server'));