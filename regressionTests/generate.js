var async = require('async');
var path = require('path');
var spawn = require('child_process').spawn;
var execFile = require('child_process').execFile;
var fs = require('fs');

/** @function safeSpawn spawns 'command args' (with options) via
 * child_process, and captures the output -- only dumping it via
 * winston if the spawned process fails with a nonzero exit code */
function safeSpawn( command, args, options, callback ) {
    var child  = spawn(command, args, options);
    
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
		callback( command + " " + args.join(' ') + " failed (exit code " + code + "; signal " + signal + ")" );
	    }, 100);
	} else {
	    callback( null );
	}
    });    
}

/** @funtion pdflatex runs pdflatex (with the appropriate ximera options) on filename */
function pdflatex( filename, jobname, options, callback )
{
    var command = '"\\PassOptionsToClass{' + options.join(',') + '}{ximera}\\PassOptionsToClass{xake}{ximera}\\PassOptionsToClass{xake}{xourse}\\nonstopmode\\input{' + path.basename(filename) + '}"';

    var cwd = path.dirname(filename);
    
    safeSpawn('pdflatex', ['-jobname=' + jobname, '-file-line-error', '-shell-escape', command],
	       { cwd: cwd },
	       callback );
}

var optionsList = require('./options-list.js');

var root = path.resolve(__dirname, 'texFiles');
var outputRoot = path.resolve(__dirname, 'pdfFiles');

var tasks = [];

var meter = require('./meter');

fs.readdir(root, function(err, items) {
    items.forEach( function(item) {
	if (path.extname(item) == '.tex') {
	    optionsList.forEach( function(options) {
		var filename = path.resolve(root, item);
		var jobname = path.basename(filename).replace(/\.tex$/,'-' + options.join('-')).replace(/-$/,'');
		tasks.push({input: filename, options: options, jobname: jobname});
	    });
	}
    });

    meter.run( tasks.length, 'pdflatexing', function( label, tick ) {
	async.eachLimit( tasks, 6,
			 function(item, callback) {
			     label(item.jobname);
			     pdflatex(item.input, item.jobname, item.options, function(err) {
				 if (err)
				     callback(err);
				 else {
				     pdflatex(item.input, item.jobname, item.options, function(err) {
					 tick();
					 var source = path.join( path.dirname(item.input), item.jobname + '.pdf' );
					 var target = path.join( outputRoot, item.jobname + '.pdf' );
					 execFile('/bin/cp', ['--no-target-directory', source, target]);
					 callback(err);
				     });
				 }	 
			     });	
			 },
			 function(err) {
			     if (err) {
				 console.log(err);
			     } else {
				 console.log("Done!");
			     }
			 });
    });
});
