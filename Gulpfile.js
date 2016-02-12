var gulp = require("gulp");
var jshint = require("gulp-jshint");

gulp.task("lint", function() {
  gulp.src(["./client/static/js/*.js", "!./client/static/js/hatohol_connector.js"])
    .pipe(jshint())
    .pipe(jshint.reporter("default"));
});
