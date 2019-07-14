import test from 'ava';

var fs = require('fs');
var path = require('path');

var optionsList = require('./options-list.js');

var path = require('path');
var execFile = require('child_process').execFile;
var child_process = require('child_process');
var fs = require('fs');

var unthrottledSpawn = function( command, args, options ) {
    return new Promise((resolve, reject) => {
	var child  = child_process.spawn(command, args, options);
    
	var output = [];

	child.stdout.setEncoding('utf8');
    
	child.stdout.on('data', function (data) {
	    output.push( data );
	});
	
	child.on('close', function (code, signal) {
	    if ((code != 0) || (signal)) {
		console.log( output.join() );	
	    
		// BADBAD: Apparently I need to wait for i/o to finish...
		setTimeout( function() {
		    reject( command + " " + args.join(' ') + " failed (exit code " + code + "; signal " + signal + ")" );
		}, 100);
	    } else {
		resolve();
	    }
	});
    });
};

var throat = require('throat');
var safeSpawn = throat(unthrottledSpawn, 6);

/** @funtion pdflatex runs pdflatex (with the appropriate ximera options) on filename */
function pdflatex( filename, jobname, options )
{
    var command = '"\\PassOptionsToClass{' + options.join(',') + '}{ximera}\\PassOptionsToClass{xake}{ximera}\\PassOptionsToClass{xake}{xourse}\\nonstopmode\\input{' + path.basename(filename) + '}"';

    var cwd = path.dirname(filename);
    
    return safeSpawn('pdflatex', ['-jobname=' + jobname, '-file-line-error', '-shell-escape', command], { cwd: cwd } );
}

function pdiff( left, right ) {
    return safeSpawn('pdiff', [left, right], {  } );
}

var root = path.resolve(__dirname, 'texFiles');
var pdfRoot = path.resolve(__dirname, 'pdfFiles');

var macros = optionsList.map( function(options) {
    function macro(t, filename) {
	var texfile = path.resolve( root, filename );
	var jobname = path.basename(texfile).replace(/\.tex$/,'-' + options.join('-')).replace(/-$/,'');

	var original = path.resolve( pdfRoot, path.basename(jobname) + '.pdf' );
	var output = path.resolve( root, path.basename(jobname) + '.pdf' );
	
	return pdflatex( texfile, jobname, options ).then( function() {
	    pdflatex( texfile, jobname, options ).then( function() {
		return pdiff( original, output );
	    });
	});
    }
    
    macro.title = (providedTitle, filename) => `${filename} with options [${options.join(',')}]`.trim();

    return macro;
});

var texfiles = fs.readdirSync(root).filter( filename => path.extname(filename) == '.tex' );

texfiles.forEach( filename => test(macros,filename) );

