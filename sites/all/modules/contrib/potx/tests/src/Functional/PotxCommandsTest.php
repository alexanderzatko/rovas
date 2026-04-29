<?php

namespace Drupal\Tests\potx\Functional;

use Drupal\Tests\BrowserTestBase;
use Drush\TestTraits\DrushTestTrait;

/**
 * @coversDefaultClass \Drupal\potx\Drush\Commands\PotxCommands
 */
class PotxCommandsTest extends BrowserTestBase {

  use DrushTestTrait;

  /**
   * {@inheritdoc}
   */
  protected static $modules = [
    'potx',
  ];

  /**
   * {@inheritdoc}
   */
  protected $defaultTheme = 'stark';

  /**
   * Tests drush commands.
   */
  public function testCommands() {
    $this->drush('potx');
    $output = $this->getOutput();
    $this->assertStringContainsString('Processing', $output);
  }

}
