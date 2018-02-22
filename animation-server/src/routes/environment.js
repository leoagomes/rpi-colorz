let express = require('express');
let router = express.Router();

/**
 * GET /env
 * 
 * returns the current environment
 */
router.get('/', (req, res) => res.send(env));

module.exports = router;