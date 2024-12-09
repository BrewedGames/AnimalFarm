-- Player Stats
current = 100
playerspeed = 10
maxSpeed = 250

-- Dash Mechanics
dashspeed = 600
dashCooldown = 1.0
dashDuration = 0.2
dashTimer = 0
canDash = true
isDashing = false

-- Direction and Movement
prevDirX = 1
prevDirY = 0
prevDir = nil

playerUp = false
playerDown = false
playerLeft = false
playerRight = false

-- Movement Physics
acceleration = 50
deceleration = 30
velocityX = 0
velocityY = 0

-- Screen and Player Dimensions
screenWidth = 1280
screenHeight = 720
playerWidth = 100
playerHeight = 100

-- Controller
CONTROLLERTHRESHOLD = 8000

-- Animation
currentAnimation = nil

-- Arrow
arrow = nil
arrowRadius = 50
arrowSprite = nil
lastAngle = 0
arrowRotationOffset = -90 