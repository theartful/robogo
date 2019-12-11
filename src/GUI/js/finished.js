var w = (c.width = window.innerWidth),
  h = (c.height = window.innerHeight),
  ctx = c.getContext("2d"),
  fireworks = [],
  tick = 0;

ctx.fillStyle = "#111";
ctx.fillRect(0, 0, w, h);

const drawTest = (text, first = false) => {
  len = ctx.measureText(text).width;
  ctx.fillStyle = "hsl(0,0%,light%)".replace(
    "light",
    50 + 20 * Math.sin(tick / 10)
  );
  if (first) {
    ctx.fillText(text, w / 2 - len / 2, h / 2 - 100);
  } else {
    ctx.fillText(text, w / 2 - len / 2, h / 2 - 10);
  }
};

function anim() {
  ctx.font = "160px Bebas Neue";
  var text = "500";
  drawTest(text, true);

  ctx.font = "60px Bebas Neue";
  var text = "Hady is the winner";
  drawTest(text);

  window.requestAnimationFrame(anim);

  ++tick;

  if (fireworks.length < 50 && Math.random() < 0.1)
    fireworks.push(new Firework());

  ctx.fillStyle = "rgba( 0, 0, 0, .04 )";
  ctx.fillRect(0, 0, w, h);

  fireworks.map(function(firework) {
    firework.step();
  });
}
function Firework() {
  this.reset();
}
Firework.prototype.reset = function() {
  this.dx = Math.random() * 300 * (Math.random() < 0.5 ? 1 : -1);

  this.sx = Math.random() * w;
  this.ex = this.sx + this.dx;
  this.ey = Math.random() * h;

  this.px = this.sx;
  this.py = 0;

  this.time = (30 + 30 * Math.random()) | 0;
  this.tick = 0;

  this.size = (2 + 2 * Math.random()) | 0;
  this.shatters = (5 + 5 * Math.random()) | 0;
  this.radiant = (Math.PI * 2) / this.shatters;

  this.shatterContainer = [];
  this.dead = false;
};
Firework.prototype.step = function() {
  if (!this.dead) {
    ++this.tick;
    var prop = this.tick / this.time,
      nx =
        this.sx + this.dx * (1 + Math.sin(-Math.PI / 2 + (prop * Math.PI) / 2)),
      ny = prop * this.ey;

    ctx.strokeStyle = "hsl(hue,80%,50%)".replace(
      "hue",
      tick / 3 + (nx / w) * 100
    );
    ctx.lineWidth = this.size;
    ctx.beginPath();
    ctx.moveTo(this.px, h - this.py);
    ctx.lineTo(nx, h - ny);
    ctx.stroke();

    this.px = nx;
    this.py = ny;

    if (prop >= 1) {
      this.dead = true;

      for (var i = 0; i < this.shatters; ++i)
        this.shatterContainer.push(new Shatter(nx, ny, this.radiant * i));
    }

    if (this.x < 0 || this.x > w) this.reset();
  } else {
    for (var i = 0; i < this.shatterContainer.length; ++i) {
      var shatter = this.shatterContainer[i];

      shatter.step();

      if (shatter.x < 0 || shatter.y > w || shatter.y > h) {
        this.shatterContainer.splice(i, 1);
        --i;
      }
    }

    if (this.shatterContainer.length === 0) this.reset();
  }
};
function Shatter(x, y, rad) {
  this.x = x;
  this.y = h - y;
  this.vx = (Math.random() + 1) * Math.cos(rad);
  this.vy = (Math.random() + 1) * Math.sin(rad);
}
Shatter.prototype.step = function() {
  var nx = this.x + (this.vx *= 0.999),
    ny = this.y + (this.vy += 0.02);

  ctx.strokeStyle = "hsl(hue,80%,50%)".replace(
    "hue",
    tick / 3 + (nx / w) * 100
  );
  ctx.lineWidth = 0.5 + Math.random();
  ctx.beginPath();
  ctx.moveTo(this.x, this.y);
  ctx.lineTo(nx, ny);
  ctx.stroke();

  this.x = nx;
  this.y = ny;
};

anim();
