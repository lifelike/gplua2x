r = 0
g = 0
b = 0

screen = gp2x.getscreen();

function nextcolor()
   r = (r + 33) % 156;
   g = (g - 33) % 156;
   b = (b + 63) % 156;
   return {r + 100, g + 100, b + 100}
end

function button(nr)
   io.write("button: ", nr, "\n")
   screen:fillrect(gp2x.rect(nr * 10, 0, 10, 10), unpack(nextcolor()))
end

function buttonup(nr)
end

function mouse(x, y)
   io.write("mouse: ", x, ",", y, "\n")
   screen:fillrect(gp2x.rect(x - 5, y - 5, 10, 10),  unpack(nextcolor()))
end

function tick(nr)
   io.write("tick: ", nr, "\n")
   gp2x.flip()
   start = gp2x.getbutton(gp2x.START)
   select = gp2x.getbutton(gp2x.SELECT)
   return start and select
end

gp2x.mainloop(1000, "button", "buttonup", "mouse", "tick")
