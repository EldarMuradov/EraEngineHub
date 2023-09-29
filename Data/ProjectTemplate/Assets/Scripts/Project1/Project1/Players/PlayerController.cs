using EngineLibrary;
using EngineLibrary.ECS;
using EngineLibrary.ECS.Components;
using EngineLibrary.Math;

namespace Project1
{
    public class PlayerController : Script, ICollidable
    {
        private Transform _transform;

        private float _health;
        public float Health 
        {
            get 
            {
                return _health;
            }
            private set
            {
                _health = value;
                if (_health <= 0.0f)
                    Die();
            }
        }

        private void Die() 
        {
            _transform.SetPosition(new Vector3D(0, 0, 0));
            GameEntity.Release();
            Debug.Log("You Died!");
        }

        public void OnCollisionEnter(Entity collision)
        {
            if (collision.Tag == "Bullet")
            {
                Health -= 10f;
            }
        }

        public override void Start()
        {
            _transform = GameEntity.GetComponent<Transform>();
            _health = 100f;
        }

        public override void Update()
        {
            //_transform.SetPosition(_transform.GetPosition() + new Vector3D(0, 10, 0 ));
        }
    }
}
