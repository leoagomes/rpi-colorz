let express = require('express');
let router = express.Router();

/**
 * GET /animations
 * 
 * returns the currently registered animations
 */
router.get('/animations', (req, res) => animations);

/**
 * GET /animations/{id}
 * 
 * {id} may be 'current'
 * 
 * returns animation data related to a specific animation
 */
router.get('/animations/:id', (req, res) => {

});

/**
 * GET /animations/{id}/parameters
 * 
 * {id} may be 'current'
 * 
 * returns the animation parameters and specification
 */
router.get('/animations/:id/parameters', (req, res) => {

});

/**
 * PUT /animations/{id}/parameters
 * 
 * {id} may be 'current'
 * 
 * sets the animation parameters that are provided as argument
 */
router.put('/animations/:id/parameters', (req, res) => {

});

module.exports = router;