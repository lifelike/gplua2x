foo = "bar"

screen = gp2x.getscreen()
image = gp2x.loadpng("libtests.png")

function clearscreen()
   if dirty then
      screen:fillrect(dirty, 111, 222, 255)
   else
      screen:clear(111, 222, 255)
   end
end

clearscreen()

x = 50
y = 50

done = false

function button(key)
   if key == gp2x.UP and y >= 10 then
      y = y - 10
   elseif key == gp2x.DOWN and y < 150 then
      y = y +10
   elseif key == gp2x.LEFT and x >= 10 then
      x = x - 10
   elseif key == gp2x.RIGHT and x < 240 then
      x = x + 10
   elseif key == gp2x.START or key == gp2x.X or key == gp2x.SELECT then
      done = true
   end
end

function buttonup(nr)
end

function mouse(mx, my)
   x = mx
   y = my
end

function tick(nr)
   clearscreen()
   dirty = screen:blit(image, x, y)
   gp2x.flip()
   return done
end

gp2x.mainloop(50, "button", "buttonup", "mouse", "tick")
io.write("the end\n");
