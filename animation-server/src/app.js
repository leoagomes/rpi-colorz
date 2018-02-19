const express = require('express');
const env = require('./env');

const app = express();

app.get('/env', (req, res) => res.send(env));

app.listen(3000, () => console.log('started server'));