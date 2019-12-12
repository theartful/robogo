var gulp = require('gulp');
var gp_concat = require('gulp-concat');
var gp_rename = require('gulp-rename');
var gp_uglify = require('gulp-uglify-es').default;
var gp_sourcemaps = require('gulp-sourcemaps');

function defaultTask() {
    return gulp.src(
        [
            "./scripts/gtp/entities.js",
            "./scripts/gtp/utils.js",
            "./scripts/gtp/protocol/gtpAdminstrativeCommands.js",
            "./scripts/gtp/protocol/gtpCorePlayCommands.js",
            "./scripts/gtp/protocol/gtpTournamentCommands.js",
            "./scripts/gtp/protocol/gtp.js",
            "./scripts/client.js",
        ]
    )
        .pipe(gp_sourcemaps.init())
        .pipe(gp_concat('main.js'))
        .pipe(gulp.dest('./scripts'))
        .pipe(gp_rename('main.min.js'))
        .pipe(gp_uglify())
        .pipe(gulp.dest('./scripts'));

}

exports.default = defaultTask;
