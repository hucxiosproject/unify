var gulp = require("gulp"),
  jshint = require("gulp-jshint"),
  gulpignore = require("gulp-ignore");

gulp.task("script-hint",function() {
  return gulp.src("src/**/*.js")
    .pipe(gulpignore.exclude("src/libs/*.js"))
    .pipe(jshint())
    .pipe(jshint.reporter("gulp-jshint-file-reporter"),{
      filename: __dirname + "jshint-output.log"
    });
});

gulp.task("default", ["script-hint"]);
